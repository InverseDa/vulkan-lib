# Vulkan Lib
![License](https://img.shields.io/badge/license-MIT-blue.svg)

Vulkan Lib is a C++ library for the Vulkan graphics API, designed to simplify the development process of Vulkan applications. It provides useful utilities and abstractions for creating and managing Vulkan resources, along with implementations of common functionality and algorithms.
## Features
- Encapsulates the creation, destruction, and management logic of Vulkan objects, simplifying resource management.
- Provides common functions and tools such as texture loading, shader compilation, and command buffer recording.
- Includes implementations of common graphics and compute algorithms that can be used directly in Vulkan applications.
## Prerequisites
- Vulkan SDK: Make sure you have the appropriate version of the Vulkan SDK installed on your machine.
- C++ Compiler: You will need a compiler that supports C++17 or higher.
## Installation
1. Clone or download the source code of this library:

```shell
git clone https://github.com/InverseDa/vulkan-lib.git
````
2. Use CMake to generate makefile or sln
```shell
cmake -S . -Bbuild
```
3. Build
```shell
cd ./build; cmake --build
```
## Usage Example
```cpp
#include "app/app.hpp"  
  
int main(int argc, char** argv) {  
    Application app{};  
  
    try {  
        app.Run();  
    } catch (const std::exception &e) {  
        std::cerr << e.what() << '\n';  
        return EXIT_FAILURE;  
    }  
  
    return EXIT_SUCCESS;  
}
```
## Contributing
Contributions and issue reports are welcome! If you find any bugs or have suggestions for improvements, please submit an issue or send a pull request.

## License
This library is licensed under the MIT License. See the LICENSE file for details.
```
Please note that this is just a rough README file template, and you should modify and supplement it according to the specifics and requirements of your library.
Make sure to provide detailed usage examples, installation instructions, prerequisites, contribution guidelines, and license information to help other developers
better understand and utilize your library.
```
