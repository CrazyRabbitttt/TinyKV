#include "memory/alloc.h"

#include <iostream>
//#include <ctime>
#include <vector>

#include "doctest/doctest.h"
using namespace tinykv;


TEST_CASE("compare allocate speed") {
  const int MAXLENGTH = 100000;
    SimpleFreeListAlloc simple_memory_pool;
    void* address = simple_memory_pool.Allocate(4);
    simple_memory_pool.Deallocate(address, 4);
    std::vector<int*> vec1(MAXLENGTH);
    std::vector<int*> vec2(MAXLENGTH);

    clock_t clock_begin = clock();
    for (int i = 0; i < MAXLENGTH; i++) {
      vec1[i] = static_cast<int*>(simple_memory_pool.Allocate(4));
    }
    for (int i = 0; i < MAXLENGTH; i++) {
      simple_memory_pool.Deallocate(vec1[i], 4);
      vec1[i] = nullptr;
    }
    clock_t clock_end = clock();
    std::cout<<"使用二级内存池，程序运行了 "<<clock_end-clock_begin<<" 个系统时钟"<<std::endl;


    clock_begin = clock();
    for (int i = 0; i < MAXLENGTH; i++) {
      vec2[i] = new int;
    }
    for (int i = 0; i < MAXLENGTH; i++) {
      delete vec2[i];
      vec2[i] = nullptr;
    }
    clock_end = clock();
    std::cout<<"直接使用系统的内存申请&释放，程序运行了 "<<clock_end-clock_begin<<" 个系统时钟"<<std::endl;

    std::cout << "Hello from memory pool test\n";
}

TEST_CASE("allocate_free_small_memory") {
    const int MAXLENGTH = 500000;
    SimpleFreeListAlloc simple_memory_pool;
    clock_t clock_begin = clock();
    for (int i = 0; i < MAXLENGTH; i++) {
      int *t = static_cast<int*>(simple_memory_pool.Allocate(4));
      simple_memory_pool.Deallocate(t, 4);
    }
    clock_t  clock_end = clock();
    std::cout << "程序运行了" << clock_end - clock_begin << " 个系统时钟" << std::endl;


    clock_begin = clock();
    for (int i = 0; i < MAXLENGTH; i++) {
      int *t = new int;
      delete t;
    }
    clock_end = clock();
    std::cout << "不使用MemPool程序运行了" << clock_end - clock_begin << " 个系统时钟" << std::endl;
}


