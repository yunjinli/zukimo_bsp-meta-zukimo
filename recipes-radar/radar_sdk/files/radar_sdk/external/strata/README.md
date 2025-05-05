# STRATA Host Library

Building locally...
-------------------

... using git-submodules without Conan (simpler)
------------------------------------------------

Check out the repository and don't forget to update the submodules (which is done automatically by most graphical git clients)
Open the CMakeLists.txt in the project root with Qt Creator (it should automatically detect the right settings and generate the project)
When building, please have a look at Projects - Build - CMake, where you have several options:
    STRATA_BUILD_SAMPLES
    STRATA_BUILD_TOOLS
    STRATA_BUILD_TESTS
    STRATA_BUILD_WRAPPERS
    STRATA_SHARED_LIBRARY

Please make sure to set them according to your neeeds.



... using Conan (recommended for experienced developers)
--------------------------------------------------------

This library depends on the "universal" headers that are in a separate git module.
Although currently that is included in the git repository under library/universal,
the recommended approach is to *not* use git submodules, instead rely on the conan system to provide the required files.
It is possible to instruct the build to use the conan version instead of the submodule, by passing
-DUSE_CONAN_UNIVERSAL=On to cmake.

Example instructions for building:

    mkdir build
    cd build
    conan install -g virtualenv -u ..
    activate.bat
    cmake -G "Visual Studio 15 2017 Win64" -DUSE_CONAN_UNIVERSAL=On ..
    cmake --build . --config Release



... using Conan with git-submodules
--------------------------------------------------------

The git-submodules needs already be checked out before using the conan command. Mainly used for the automatic release procedure for Strata.

Example instructions for building:

    mkdir build
    cd build
    conan install -g virtualenv -u .. -o submodules=True
    activate.bat
    cmake -G "Visual Studio 15 2017 Win64" ..
    cmake --build . --config Release
