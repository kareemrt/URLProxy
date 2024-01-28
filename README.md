# URLProxy

URLProxy is a light-weight library that allows users to make HTTP GET requests using a Socks5 proxy and customized browser headers. A new proxy and header are rotated upon each request.

URLProxy is available as both a Python and C++ library. The C++ library includes a PyBind11 attachment so that the library can be used/imported into python.

For Python, URLProxy minimally extends the requests' module 'get' method functionality; for C++, URLProxy extends the libcurl library.

The purpose of randomly rotating proxies and headers on web-requests is to circumvent server-side blocks; this includes a 'BadIPs' feature, which will remove unresponsive/broken proxies from the selection pool.

## Install Instructions

#### Python Library
- Option 1 (Easy): Install Wheel directly (shell: 'pip install git+https://github.com/kareemrt/URLProxy/Python/dist/URLProxy-[currentversion]-py3-none-any.whl')
- Option 2 (Easy): Build the wheel from github and install it yourself (shell: 'pip install git+https://github.com/kareemrt/URLProxy/Python.git')
- Option 3 (Medium): Download, build, & install the wheel yourself
  1. Download the Python folder
  2. Run 'python setup.py sdist bdist_wheel'
  3. Retrieve the '.whl' file in Python/dist and install to your desired venv using 'pip install {file name}.whl'
- **NO INSTALL Option 4 (Easy):** Download and import the /Python/URLProxy.py module into your desired project

#### C++ Library
**The C++ module was created as a faster alternative to the Python module and can be used within Python code; the instructions differ between using the C++ library in C++ or the C++ library in Python.**
- For use in C++, you may:
  - Option 1 (Medium): Recompile the source code into a library yourself using a >= C++17 compiler
    - g++, msvc, clang, etc.
  - Option 2 (Easy): Link to a static library included in: C++/prebuilt_modules/URLProxyC.{ext} (.a for Linux/MacOS, .lib for Windows)
    - Include the header file at C++/include/URLProxyC.h
  - Option 3 (Easy): Include the dynamic library located at: C++/prebuilt_modules/URLProxyC.{ext} (.so for Unix, .dll for Windows)
  
- For use in Python, you may:
  - Option 1 (Medium): Recompile the source code into a .so(Unix)/.dll(Windows) dynamic library yourself using a >= C++17 compiler
    - g++, msvc, clang, etc.
    - Place the dynamic library file in your project directory either include 'import URLProxyC' in your python module or use ctypes if that does not work.
  - Option 2 (Easy-Medium): Recompile the source code into a .pyd PyBind11 library yourself using setup_tools
    - 'Python setup.py build_ext --inplace'
    - Place the .pyd file in your project directory and include 'import URLProxyC' in your python module
  - Option 3 (Easy): Import the prebuilt .pyd library included in: C++/prebuilt_modules/URLProxyC.[extrastuff].pyd
    - Place the .pyd file in your project directory and include 'import URLProxyC' in your python module

## Use Instructions

1. Create a 'credentials.json' file with the following information (a sample has been included):
```json
{
"credentials": ["socks5 username:socks5 password"],
"headers": ["header1", "header2",...],
"proxies": ["proxy1", "proxy2",...]
}
```
2. Place 'credentials.json' in the ROOT of your project directory
   - To place elsewhere, call .set_credentials_fpath(str: filePath) after import (e.g., URLProxy.set_credentials_fpath("./IO/credentials.json"))
3. Import the corresponding library (Python/C++), call library.force_connect(str: url, int: max_tries_per_proxy)
4. Smile
   
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
