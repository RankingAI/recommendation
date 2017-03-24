# Description
This project will end up with embracing recommendation service with high performance, data processing, model learning, and simulated request client. All relevant framework or technics will be tested and included. 

C++ will be the kernel language, at the meantime features of c++11 and boost will included.

Recommendation online service is under going, the other modules will be brought up here after then.

### Prerequisite
_At first, make sure the version for _cmake_ is not less than 3.5 and _c++_ is not less than 11, in addition _gcc_ compiler is a must.
* **thrift**, version 0.93, service deployment tool
* **hiredis**, version 0.13, redis interface wrapped with c
* **boost**, version 1.6.0, third-party c++ library featured _functional programming_
* **jsoncpp**, version 1.8.0, _json_ interface wrapped with _c++_
* **libevent**, version 2.2.0, event actor framework for tcp request

### Building
* cd RankingCpp && mkdir build && cd build
* cmake -D CMAKE_C_COMPILER=/where/is/your/gcc -D CMAKE_CXX_COMPILER=/where/is/your/g++ ..
* make

### Run server-side
./ServerRun.sh
### Run client-side
./ClientRun.sh
