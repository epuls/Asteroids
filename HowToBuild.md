# Build Instructions (Windows/CMake)
## Dependencies:
- SDL3 (extern folder)
- SDL3_ttf (extern folder)
- stb_image (extern folder)
- glad (extern folder)


## Create MakeFile With CMake
1. Using CMake (GUI):
   2. Point to this folder as the source code
   3. Select the folder you would like to build binaries
4. Hit configure and select the MinGW Generator
5. Hit Generate

# In the MSYS2 UCRT64 shell
1. Navigate to the directory you configured your MakeFile in and build:
`cmake --build .`


# Note
Resources should be copied directly to the build folder automatically, if any errors are thrown regarding texture loading double check the resources folder is included in the same directory as the executable