# AppDynamics IoT C++ SDK

[![Build Status](https://travis-ci.org/jimix/iot-cpp-sdk.png?branch=master)](https://travis-ci.org/jimix/iot-cpp-sdk)

## Overview
This repository contains AppDynamics IoT C++ SDK that can be used in embedded applications to monitor network perfomance, errors and business metrics.


## Contents

* `sdk` - C++ SDK source and headers
* `sample` - Sample Application showing how to use the SDK to capture and send events
* `tests` - Unit Tests to test functionality of SDK
* `docs` - SDK API document built in html format using Doxygen.


## Requirements

* GNU C++ Compiler (g++) version 4.2 or above for 32/64-bit architectures
* Any Linux x86 distribution based on glibc 2.20+
* If using macOS - LLVM version 8.0.0 or above
* cmake version 3.0 or above

Before building the sdk, check if cmake is installed. If not, you can get it using package manager

```sh
$ sudo apt-get install cmake
$ cmake -v
```
Also, ensure your system has all the tools (g++, make, libc, git etc) to build c++ code. If not, you can install them using

```sh
$ sudo apt-get install build-essential
$ sudo apt-get install git
$ sudo apt-get install lcov
```
On 32 bit machines, you might also need multilib

```sh
$ sudo apt-get install gcc-multilib g++-multilib
```

## Dependencies

* cgreen - Tests are written using [cgreen](https://github.com/cgreen-devs/cgreen) unit testing framework to verify SDK functionality.
  cgreen repo will be downloaded, compiled and linked to tests as part of build step.
* curl - Sample application uses [curl](https://curl.haxx.se/) to send events to Appdynamics IoT Collector

You can install curl and its headers using below commands:

```sh
$ sudo apt-get install curl
$ sudo apt-get install libcurl4-gnutls-dev
```

## Platforms

SDK has been built and tested on below platforms.
* Linux x86 32/64 bit architectures
* macOS 10.12 and above

SDK can be built on other platforms as long the compiler has support for ISO C++ standard C++98/C++03.

## Build Targets

Below are the targets that can be built using this repository.

* `appdynamicsiotsdk` -  SDK is built as a library(libappdynamicsiot) by default. The library and headers can be found in lib/ and include/ directories created when this target is built.
* `sample` - Sample application is built as executable `sample` by default.
* `tests` - Tests are built as executable linking cgreen library.


## <a id="Installation"></a>Installation

All the targets can be built from root directory using the below commands.

```sh
$ git clone https://github.com/Appdynamics/iot-cpp-sdk.git
$ cd iot-cpp-sdk
$ mkdir build
$ cd build
$ cmake ..
$ make
```

if `build` directory is created outside of the root directory, then pass the path of root directory to cmake command.

Only sdk and sample targets are built by default. To build tests, run

```sh
$ cmake .. -DBUILD_TESTS=1
$ make
```

If you want to view the code coverage done by the tests, run

```sh
$ cmake .. -DENABLE_COVERAGE=1
```
Code coverage has a dependency on `gcov`, `lcov` & `genhtml`.

If you want to build a 32 bit library on a 64 bit machine, set the flag DBUILD_32BIT

```sh
$ cmake .. -DBUILD_32BIT=1
$ make
```

You can also build individual targets using below commands

```sh
$ make appdynamicsiotsdk
$ make sample
$ make tests
$ sudo make run-code-coverage
```

You can run tests from the `build` folder by using commands below
```sh
$ make ./run-tests
```

You can run tests from the `build` folder by using commands below
```sh
$ make ./run-code-coverage
```

You can view the code coverage report from the `build` folder by opening the
`out/index.html` file

## How to Use

Best place to understand the usage of SDK is to run the sample application. Please refer [README](sample) of sample application for steps to run.

Below are the steps to integrate and use SDK within your own application:

1. Dynamically link SDK Library (libappdynamicsiot) to your application
    * You can find the library `libappdynamicsiot` in lib/ folder once sdk target is built
    * If you are using cmake, refer to CMakeLists.txt within sample/ on how sdk library is linked
2. Copy and import SDK headers from include directory into your application
3. Get [EUM App Key and Collector URL](https://docs.appdynamics.com/display/PRO44/Set+Up+and+Access+IoT+Monitoring#SetUpandAccessIoTMonitoring-iot-app-key)
4. Initialize SDK with App Key and Collector URL before adding and sending data
5. Confirm [IoT Data](https://docs.appdynamics.com/display/PRO44/Confirm+the+IoT+Application+Reported+Data+to+the+Controller) is
reported to Collector


## Additional Resources

1. [SDK API docs](docs) - provides definition and implementation of SDK API
2. [REST API docs](https://docs.appdynamics.com/javadocs/iot-rest-api/4.4/latest/) - provides payload structure sent to IoT Collector
3. [User Documents](https://docs.appdynamics.com/display/PRO44/IoT+Monitoring) - provides overview on IoT monitoring capabilities
4. Sample Application in other languages are available on github
    * [Java](https://github.com/Appdynamics/iot-java-sdk-sample-apps)
    * [Python](https://github.com/Appdynamics/iot-rest-api-sample-apps)
