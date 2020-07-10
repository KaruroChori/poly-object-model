# Introduction
Designing self modyfing code or at least code which can be hot-plugged in our applications is very simple for interpreted languages but quite harder in the case of compiled ones.
The most common technique is to generate the code, compile and link it as a library, and to load it in the final application, providing all the bindings to the symbols we specified.
This process can be quite annoying, it is easy to introduce mistakes and it is hard to enforce that the interface is exactly the same between the host code and the dynamic library.

This repository contains a small set of utilities to automatize most of this process and the specifications for two JSON format introduced to represent the interface and its implementation.
These formats are language and platform-agnostic, however at the moment the only language supported is C++ in a modern version (at least 2014, but it was only tested from 2020 on).
Dependecies are kept at a minimum. In the future the scope of the actual implementation can be widely improved.

These are the distributed utilities:
- **gen-host** does generate the host code to load an external libraries and all the symbols according to the specifications.
- **gen-dl** generates the library code with an interface which is compatible with the one enforced by gen-host.
- **impl** generates a workspace based on a specification file to simplify the implementation phase.
- **merge** does merge together all the files in a workspace to generate a more distributable implementation json file.
- **from-code** does read your code and automatically create the equivalent specification and the implementation files.

You can find the details of the specifications for the JSON files in the *doc* directory.

# Features
At the moment only **gen-host** and **gen-dl** have a partial working implementation. This is what is planned for the future:
- Add support for the "type" method in packing variables.
- Add the utilities **impl** and **merge**.
- Implement some proper test cases.
- Add some code to the test_xxx functions which at the moment do not perform any real check.
- Implement **from-code** (it will take longer than anything else really)
- Expand to work with some other language. Maybe.
- Built-in code beautifier.

# Directory structure
Directories are organized in the following structure:
- **opt**: the external components.
- **src**: the actual C++ files to be compiled into the exported library.
- **apps**: C++ subprojects to be compiled and linked based on the exported library.
- **test-suite**: C++ sub-projects used to test the library.
- **benchmark-suite**: C++ sub-projects used to test performance of the library.
- **scripts**: folder with utility scripts.
- **header**: .h files for this framework.
- **docs**: the documentation of the project.
- **private**: temporary storage for uncompleted stuff stored only locally.

# Compiling
The compiling process is based on cmake. Please check you have a recent version installed on your system and a proper toolchain.
It is also suggested to have **clang-format** or any other prettyfier installed as the code generated by these tools is not well formatted.
>
`git submodule update --init --recursive`
`mkdir build`
`cd build`
`cmake -D CMAKE_C_COMPILER=clang-10 -D CMAKE_CXX_COMPILER=clang++-10 -D CMAKE_CXX_FLAGS="-stdlib=libc++" ..`
`make all -j8`

You can also use `make test` to execute all the tests the library comes with, and `make doxygen` to generate its documentation.

By default cmake does not support any target yet. If you want to generate faster code you could enable O3 optimizations:
>
`cmake -D CMAKE_C_COMPILER=clang-10 -D CMAKE_CXX_COMPILER=clang++-10 -D CMAKE_CXX_FLAGS="-stdlib=libc++ -O3" ..`

# Licence
You are free to use/modify/implement/whatever the specifications of the JSON formats freely, however you shall not change and distribute them under the same name. This is to avoid potential confusion with the main release.
You are free to use the code generated by these utilities as you prefer in any kind of application and no liability on my side.
This code however is not yet released under a permissive licence. Still, bugfixies and contributions are welcome and your name as a contributor will always be displayed.
A more permissive licence will come at some point in the future.