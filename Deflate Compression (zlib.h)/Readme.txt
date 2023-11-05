This code employs an external C/C++ header file - zlib.h for employing image compression.
This library must be pre-installed in order to run the code.

To install the zlib.h header file on a linux system, open terminal and run the following commands -
sudo apt-get update
sudo apt-get install zlib1g-dev

At the time of compilation, the header file "zlib.h" must be linked to the programme by the "-lz" flag -
g++ Deflate.cpp -lz

The program reads a BMP image named "i1.bmp". Ensure that such an images is present in the running directory before execution.

The program outputs a binary file named "compressed_data.bin" which contains the image data after compression.
