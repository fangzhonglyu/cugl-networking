# CMAKE BUILD DIRECTORY

When the SDL script configures a CMake build, it only creates the CMake file. It does not install it or use it to compile the build. That is because executing CMake is extremely not cross-platform. That is why we recommend against sharing your CMake files in your Code repository.

The CMake file that you need is stored in `src.dir`.  To perform the build, open a terminal to this directory and perform the following two commands in order:

```
cmake src.dir
cmake --build .
```
