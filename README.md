## Table of Contents
* [Description](#description)
* [Deployment](#deployment)
 * [Prerequisite](#prerequisit)
	* [Configuration for Environment Variables](#configuration-for-environment-variables)
	* [Installation for cmake](#installation-for-cmake)
  	* [Installation for Boost](#installation-for-boost)
  	* [Installation for libevent](#installation-for-libevent)
  	* [Installation for thrift](#installation-for-thrift)
  	* [Installation for jsoncpp](#installation-for-jsoncpp)
  	* [Installation for hiredis](#installation-for-hiredis)
 * [Building](#building)
* [Startup of server-side](#startup-of-server-side)
* [Startup of client-side](#startup-of-client-side)

## Description 
Ranking service written with _c++11_, is a distributed service deployed with _thrift_ supporting non-block response mode for high-concurrency tcp requests.

## Deployment ##

#### Prerequisit ###
First of all, you will be informed that the following steps work well on ubuntu14.0.4 with gcc compiler(if you choose clang as your c/c++ compiler, you'd better to reflect on the context of your own OS before continuing), make sure that c++11 is supported.

### Configuration for Environment Variables
* open up the directory where your **binary** files reside, export PATH=/usr/local/bin:$PATH
* open up the directory where your **library** files reside, export LD\_LIBRARY\_PATH=/usr/local/lib:/usr/lib:$LD\_LIBRARY\_PATH

#### Installation for cmake
Cmake is used to build, test and package c++ project here.
* **Download** source code, wget from [its official website](https://cmake.org/files/v3.8/cmake-3.8.0-rc4-Linux-x86_64.tar.gz)
* **Decompress** package, tar -zxvf cmake-3.8.0-rc4-Linux-x86\_64.tar.gz && cd cmake-3.8.0-rc2
* **Configure** context, ./configure —prefix=/usr/local
* **Compile and install** it, make && sudo make install
* **Check** whether success or not, which cmake && cmake --version

#### Installation for Boost 
Boost is a powerful third-party library, used by most frameworks or tools developed by c++ here.
* **Clean** older versions of it, sudo rm -r /usr/lib/x86\_64-linux-gnu/\*boost\* /usr/lib/\*boost\* /usr/local/lib/\*boost\* /usr/include/\*boost\* /usr/local/include/\*boost\*
* **Download** source code, wget from [its official website](https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.bz)
* **Decompress** package, tar -jxf boost\_1\_63\_0.tar.bz2
* **Configure** context, cd boost\_1\_63 &&  ./bootstrap.sh
* **Compile and install** it, ./b2 install —prefix=/usr/local/

#### Installation for libevent
A socket high-efficient handler framework based on event-driven mechanism, used by thrift with non-block sever mode here.
* **Download** the newest version of libevent from [its offical website](http://libevent.org/)
* **Decompress** package, tar -zxvf libevent\*.tar.gz
* **Configure** context, cd libevent\* && mkdir build && cmake ..
* **Compile and install** it, make && sudo make install

#### Installation for thrift
A framework used for deploying distributed service here, supporting protocol(\*.thrift file) and calling service remotely here.
* **Download** the newest version of thrift from [its offical website](https://thrift.apache.org/)
* **Decompress** package, tar -zxvf thrift\*.tar.gz 
* **Configure** context, cd thrift\* && ./configure --prefix=/usr/local/ --with-c\_glib=yes —with-ruby=no

  > Be aware that the state of "Build TNonblockingServer" in the summary list is "yes", otherwise you would better to check and assure that libevent is correctly installed before continuing 

* **Compile and install** it, make && sudo make install

#### Installation for jsoncpp
Json parser wrapped with c++, used to parse request and wrap response with json format here.
* **Download** source code, git clone from [its github home page](https://github.com/open-source-parsers/jsoncpp.git)
* **Configure** context, cd jsoncpp && mkdir build && cd build && cmake ..
* **Compile and install** it, make && sudo make install

#### Installation for hiredis
Redis interface wrapped with c, used to retrieve model or anything else here.
* **Download** soruce code, git clone from [its github home page](https://github.com/redis/hiredis.git)
* **Compile and install** it, cd hiredis && make && sudo make install

### Building
* **Download** source code, git clone from [its gitlab home page](https://github.com/RankingAI/recommendation.git)
* **Configure** context, cd ad-dm/RankingCpp && mkdir build && cd build && cmake ..
* **Compile** it, make && cd ..

## Startup of server-side
cd ad-dm/RankingCpp && chmod u+x ./ServerRun.sh && ./ServerRun.sh
> If you have no setting privilege for command _ulimit_, you can temporarily uncomment the first line in this script within the circumstance that an arbitary process have enough number of socket IO handle in your host.

## Startup of client-side
cd ad-dm/RankingCpp && chmod u+x ./ClientRun.sh && ./ClientRun.sh
> If you have no setting privilege for command _ulimit_, you can temporarily uncomment the first line in this script within the circumstance that an arbitary process have enough number of socket IO handle in your host.
