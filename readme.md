# Helix-C
This is a C interface to Twitch's API.

# Installation
1. `git submodule init`
2. `git submodule update`
3. `mkdir build && cd build`
4. `cmake ..`
    - run with `-DBUILD_SHARED_LIBS=OFF` to build static lib
5. `make install`

* The default install location is `/usr/local/lib` and `/usr/local/include/helix`

# Dependencies
- [json-c](https://github.com/json-c/json-c) for json handling
- [libcurl](https://curl.se/libcurl/) for requests (should be already available on Mac and Linux)

# How to use
To use this project, simply `#include <helix/helix.h>` and link with `-lhelix`, `-ljson-c`, and `-lcurl`
