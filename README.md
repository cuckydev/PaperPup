# PaperPup
PaperPup is a very early PaRappa the Rapper clone written in C++. It is currently in development and is not yet playable.
 
## Building
To compile the project, simply compile it the same as any other CMake project.

Using Visual Studio, just opening the project folder should be enough to build the project.

Using CMake in a terminal, it should be as simple as...

Configuring the project: (Only need to do this once or to change a setting such as build type)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

Then building the project:
```bash
cmake --build build
```

This project depends on [Leon](https://github.com/cuckydev/Leon), which itself depends on LLVM libclang 16.0.0+. More information can be found [here](https://github.com/cuckydev/Leon/blob/main/README.md).
