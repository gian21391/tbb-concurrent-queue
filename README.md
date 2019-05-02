# TBB Concurrent Queue (header only)

[![Build Status](https://travis-ci.com/gian21391/tbb-concurrent-queue.svg?branch=master)](https://travis-ci.com/gian21391/tbb-concurrent-queue) [![Apache License Version 2.0](https://img.shields.io/badge/license-Apache_2.0-green.svg)](LICENSE) [![Stable release](https://img.shields.io/badge/version-2019_U5-green.svg)](https://github.com/01org/tbb/releases/tag/2019_U5)

TBB Concurrent Queue is a header-only version of the Concurrent Queue provided by the library [Threading Building Blocks (TBB)](https://www.threadingbuildingblocks.org/).

### Usage

```c++
#include <tbb/header_only/concurrent_queue.h>
#include <iostream>

int main() {

  tbb::concurrent_bounded_queue<int> queue;

  for (int i = 0; i < 10; i++)
  {
    queue.push(i);
  }

  int n;
  while (!queue.empty())
  {
    queue.pop(n);
    std::cout << n << std::endl;
  }

  return 0;
}
```

### Documentation

Read the full documentation on the [Intel Website](https://software.intel.com/en-us/tbb-tutorial)

[//]: # (Comments) 

