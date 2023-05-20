# Lab 5

This lab uses [Atlas]() to explore writing static analysis tools to find vulnerabilities. The vulnerable MiniShare web application provided depends on Windows header files and the g++ compiler. Since the lab environment is provided as a Linux virtual machine, an Atlas codemap has been pre-computed in a Windows environment so that it may be loaded on the lab environment.

The Atlas codemap was computed using the following:
- Atlas for Windows: https://ensoftcorp.com/downloads/atlas/EnSoft-Atlas-win64+CDT-3.9.2.zip
- MinGW: https://sourceforge.net/projects/mingw/files/latest/download (with mingw-developer-toolchain, mingw32-base, mingw32-gcc-g++ installed)

Note that the g++ compiler has change slightly from the original source provided in the zip file. In order to make compilation succeed a few minor syntax changes were made (e.g. to fix a struct declaration) which are provided in the (minishare)[minishare] Eclipse project create for this lab.
