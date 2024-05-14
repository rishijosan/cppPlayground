#include <fstream>
#include <random>
#include <chrono>
#include <iostream>
#include <cstring>
#include <omp.h>

#define NUM_THREADS 4

void getRandBuf(std::string& buffer, size_t bufSize){

  const char alphanumeric[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  // Initialize random number generator
  std::random_device rd;

  //Parallelize over 4 threads and keep a separate random_device per thread
  #pragma omp parallel num_threads(NUM_THREADS) private(rd)
  {
    int tid = omp_get_thread_num();
    std::mt19937 gen(rd() + tid); 
    //std::default_random_engine gen(rd());

    //std::uniform_int_distribution<char> dis;
    static std::uniform_int_distribution<> dis(0, strlen(alphanumeric) - 1);
    
    // Generate random data and write to file
    for (size_t i = 0; i < bufSize; i++) {
        buffer[i] =  alphanumeric[dis(gen)];
        //buffer[i] =  alphanumeric[tid];
    }
  }
}


int main() {
  // Set file size in bytes
  const size_t bufSize = 1024*1024*512;
  std::string buffer;
  buffer.reserve(bufSize);

  auto start_time = std::chrono::high_resolution_clock::now();
  getRandBuf(buffer, bufSize);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::nano> elapsed_time = end_time - start_time;
  std::cout << "Time taken generating rand: " << elapsed_time.count() / 1e9 << " seconds" << std::endl;


  // Open file for writing in binary mode
  std::ofstream file("random_data.bin", std::ios::binary);
  // Check if file open was successful
  if (!file.is_open()) {
    // Handle file open error
    return 1;
  }

  start_time = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < bufSize; i++) {
    file << buffer[i];
  }
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - start_time;
  std::cout << "Time taken writing to disk: " << elapsed_time.count() / 1e9 << " seconds" << std::endl;

  file.close();

  return 0;
}