NTTW Library only needs C standard library, build projects are provided for CMake 2.6+
which is a cross-platform build tool. One can generate makefiles and Codeblocks, Visual Studio and Eclipse projects
from CMake.

BUILD
-----------------------------------------------
Simply create a build directory, then run ccmake in that directory or cmake-gui from anywhere. For ccmake, it looks like

 $ cd /path/to/nttw-source/
 $ mkdir build
 $ cd build
 $ ccmake ..
 
After configuring as desired, on Linux with GCC:
 $ make -j 4

Or on Windows, via the command line:
 $ jom /j 4 or nmake
     
For more help, consult any CMake guid or manual.

-----------------------------------------------
The Doxygen configuration is stored in file: nttw.doxy
