# AOE Greens Hardware Control Server

This plugin is used to control hardware over a network.

We have provided this code to you in the hopes of making sustainable food production a world-wide reality. For more information on our open source software and how to reach us, please visit [https://aoegreens.com/about/](https://aoegreens.com/about/).

## Usage

This API server is a webserver wrapper around the [AOE Greens Hardware Library]()

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

This project depends on the [eons Basic Build System](https://github.com/eons-dev/bin_ebbs), which can be installed with `pip install ebbs`.  
With that said, this is just c++ code. Build it however you want.

Build using ebbs:
```
cd build
ebbs -c build_local.json
```

installation instructions coming soon (need package manager / repository semantics for module delivery)

### Dependencies

Before building this, make sure you have built or installed the following dependencies

For this project, we currently use:
 * [Develop Biology](https://github.com/develop-biology/lib_bio)
 * [AOE Greens Hardware](https://github.com/aoegreens/lib_hw)
 * [Restbed](https://github.com/Corvusoft/restbed)
 * [CPR](https://github.com/libcpr/cpr)
 * [tomykaira/Base64.h](https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594) (Included in project)


