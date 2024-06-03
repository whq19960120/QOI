 ![language](https://img.shields.io/badge/language-C-green.svg) ![build](https://img.shields.io/badge/build-Windows-blue.svg) ![build](https://img.shields.io/badge/build-linux-FF1010.svg)

　

Simple QOI compressor/decompressor
===========================

**QOI** (Quite Okay Image) is a simple lossless RGB/RGBA image compression format. You can find QOI specification and reference software compressor/decompressor from [Official website of QOI](https://qoiformat.org/) . 

This repo is a simple QOI compressor/decompressor in C language, only 240 lines of C. It can:

- Compress a raw uncompressed PPM image file to QOI file.
- Decompress a QOI file to a PPM image file.

> Note: The PPM format is a very simple RGB image file format which contains the uncompressed raw RGB pixels. The format description of PPM can be found at : https://netpbm.sourceforge.net/doc/ppm.html

　

# Compile

Run command:

```bash
gcc src\*.c -o qoi_codec.exe -O3 -Wall
```

which will get *qoi_codec.exe* 

　

# Usage

An example to compress a PPM image file to a QOI file:

```bash
.\qoi_codec.exe ppm_image\05.ppm 05.qoi
```

An example to decompress a QOI file to a PPM image file:

```bash
.\qoi_codec.exe 05.qoi 05.ppm
```

　

　

# Related links

-  [Official website of QOI](https://qoiformat.org/) ：you can find QOI specification and software code here.
-  [github.com/WangXuan95/FPGA-QOI](https://github.com/WangXuan95/FPGA-QOI) : a FPGA-based QOI compressor/decompressor in Verilog language.
