# Hardware Control Server for Raspberry Pi

This plugin is used to control AOE Greens hardware.

We have provided this code to you in the hopes of making sustainable food production a world-wide reality. For more information on our open source software and how to reach us, please visit [https://aoegreens.com/about/](https://aoegreens.com/about/).

## Usage

This API server is intended to run on a Raspberry Pi or other device which implements WiringPi (if there are any).

Currently all requests must use `GET`. Unfortunately, the json library we were using did not work on our test devices. Until that is fixed, we cannot take JSON `POST` requests.

This API dynamically provisions GPIO controllers. Thus, you do not need to initialize some sort of hardware mapping, simply make the calls to read and write what you want and if we haven't started that device before, we will do so just-in-time.

Full documentation on the parameters available will be coming soon(ish).
For now, you'll want to refer to `main.cpp`.
However, to get you started, most of what you'll want will be something like:
```cpp
cpr::Response upstreamResponse = cpr::Get(cpr::Url("http://TARGET_DEVICE/v1/gpio/get?pin=SOME_PIN"));

```
Or
```cpp
cpr::Response upstreamResponse = cpr::Get(cpr::Url("http://TARGET_DEVICE/v1/gpio/set?pin=SOME_PIN&state=TOGGLE"));

```

PLEASE NOTE: pin numbers are based on the WiringPi numbering: https://pinout.xyz/pinout/wiringpi#

## Building

This code is intended to be built with [EBBS](https://github.com/eons-dev/bin_ebbs).

### Dependencies

Before building this, make sure you have built or installed the following dependencies

For this project, we currently use:
 * [WiringPi](https://github.com/WiringPi/WiringPi)
 * [Restbed](https://github.com/Corvusoft/restbed)
 * [CPR](https://github.com/libcpr/cpr)
 * [tomykaira/Base64.h](https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594) (Included in project)

### With EBBS

To build this code locally, you can run:
```shell
pip install ebbs
ebbs
```
Make sure that you are in the root of this repository, where the `build.json` file lives.

### Without EBBS

All ebbs really does is dynamically generate a cmake file for us. So, if you'd like to build this project without it, you'll just want to create a cmake file (or use whatever other build system you'd like).

A cmake produced by ebbs might look like:
```cmake

cmake_minimum_required (VERSION 3.1.1)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /opt/git/srv_pi_controller/./build/entrypoint)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /opt/git/srv_pi_controller/./build/entrypoint)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /opt/git/srv_pi_controller/./build/entrypoint)
project (entrypoint)
include_directories(/opt/git/srv_pi_controller/inc)
add_executable (entrypoint /opt/git/srv_pi_controller/src/main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Threads::Threads)
target_link_libraries(entrypoint restbed cpr)
