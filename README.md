# URLProxy

URLProxy is a light-weight library helping users make HTTP GET requests by loading a new socks5 proxy and browser header upon each request.

Both Python/C++ options are available. The C++ library can be directly imported into python via the PyBind11 module (.pyd file).

URLProxy extends the 'requests' module in Python, 'libcurl' module in C++.

A bad IP pool feature is included to remove dead proxies/speed up performance.

## Install Instructions

#### Python Library
- (Prebuilt-install): **Install directly** ('pip install git+https://github.com/kareemrt/URLProxy/Python/dist/URLProxy-[currentversion]-py3-none-any.whl')
- (Self-install): **Build wheel from git, install yourself** ('pip install git+https://github.com/kareemrt/URLProxy/Python.git')
- (Self-install): **Download, build, & install wheel yourself**
  1. Download the Python folder
  2. Run 'python setup.py sdist bdist_wheel'
  3. Retrieve the '.whl' file in Python/dist and install to your desired venv using 'pip install {file name}.whl'
- (No install): **Download, import /Python/URLProxy.py module into desired project**

#### C++ Library
*The C++ module was created as a faster alternative to the Python module and can be used in Python; the C++ library use instructions differ between Python/C++.*
- C++:
  - (Recompile - Hard): **Compile source yourself (>=c++17)**
    - g++, msvc, clang, etc.
    - dependencies: jsoncpp, libcurl, pybind11 (if using in python)
  - (Build - Medium): **Use build system to manage depenedencies, compile**
    - CMAKE
  - (Link - Easy): **Link prebuilt library**
    - If static, link: C++/prebuilt/URLProxyC.{.a/.lib}  (Unix/Windows)
      - include: C++/include/URLProxyC.h
    - If dynamic, link: C++/prebuilt/URLProxyCdyn.{.so/.dll} (Unix/Windows)
  
- Python:
  - (Recompile - Hard): **Compile source into dynamic library yourself (>=C++17)**
    - g++, msvc, clang, etc.
    - Dyn library -> project directory. Use ctypes. Declare extern if things don't work.
  - (Build - Medium): **Manage dependencies, use build system to compile**
    - *setup_tools*: Generate .pyd lib using PyBind11
      - 'Python setup.py build_ext --inplace'
      - .pyd -> project directory -> 'import URLProxyC'
    - *CMAKE (UNIX only)*: Generate .so lib
      - import using ctypes
  - (Prebuilt - Easy): Import prebuilt library 
    - C++/prebuilt/URLProxyC.[ver_info].pyd
      - .pyd -> project directory -> 'import URLProxyC'
    - C++/prebuilt/URLProxyC.[ver_info].so
      - import using ctypes

## Use Instructions

1. Create 'credentials.json' file with following (sample included):
```json
{
"credentials": ["socks5 username:socks5 password"],
"headers": ["header1", "header2",...],
"proxies": ["proxy1", "proxy2",...]
}
```
2. Place 'credentials.json' in ROOT of project directory
   - For elsewhere, call .set_credentials_fpath(filePath) after import
3. Import library (Python/C++), call .force_connect(str: url, int: tries)
4. Enjoy
   
## CMake

*Here is a CMakeLists.txt file if you choose to rebuild the library using a build system*

```cmake
cmake_minimum_required(VERSION 3.10)
set(PYBIND11_PYTHON_VERSION "3.10")
set(CMAKE_CXX_STANDARD 20)

project(URLP)
add_library(URLProxyC SHARED URLProxyC.cpp) # replace SHARED with STATIC if you want a STATIC library

target_include_directories(URLProxyC PRIVATE /usr/include/python3.10)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

find_package(jsoncpp REQUIRED)
target_link_libraries(URLProxyC PRIVATE jsoncpp)

find_package(CURL REQUIRED)
target_link_libraries(URLProxyC PRIVATE CURL::libcurl)
```

## Version History

- 0.13 (Current): C++ release, credentials check changes
- 0.12 (12/6/23): BadIPs feature prints added for transparency
- 0.11 (11/15/23): BadIPs feature introduced to remove redundant IPs and increase performance
- 0.1 (11/13/23): URLProxy v0.1 released

## About
###  Author: Kareem T | Date: 11/13/2023

This project was created as a module for another project, then was later extended to become its own library. I am an amateur software engineer, so the code may not be extremely sophisticated; any feedback is appreciated.

## Legal

I do not condone the use of this product for illegal purposes.
