What is it?
--------------------
This is a very simple tool to collect a folder full of .ico images and stick them into a single .ico file with multiple resolutions.


What Icon sizes should I supply?
--------------------
Icon rendering in Windows is complicated. Windows renders icons in various sizes in various places, and that number is compounded by DPI and zoom settings.

Due to that complexity and also bugs in Windows, the suggested sizes are 16, 20, 24, 28, 30, 31, 32, 40, 42, 47, 48, 56, 60, 63, 84 and 256 pixels square. Hopefully Microsoft will eventually fix the bugs to reduce the list, and perhaps support SVG icons as well.


Building
--------------------
I use the init / clean scripts with meson / ninja to build, but you don't have to use any of that. Simply compile main.cpp as C++/17 or higher in any C++ compiler with linux headers available. No linking is required. If you're developing in Windows, please try the Windows Subsystem for Linux (WSL) feature. This code could be ported to Windows APIs easily, a novice developer could do it in a day.


Expected Input
--------------------
This is a command line tool.

Use:
icoPack prefix [path]

The optional path parameter indicates where all files will be read/written. If not supplied it will use the current working directory.

The prefix parameter controls which files are read, and the name of the output file. For example:

    > ./icoPack "logo dark" logos/

where the logos/ directory contains:

    logo dark 16.ico
	logo dark 20.ico
	logo dark 24.ico
	...
	logo dark 256.png

The program will read those icons, sort them, and output `logo dark.ico`.

File names don't have to have be numbered by size, however icoPack will sort the names from shortest to longest, with traditional string sorting within each length.


Large Images
--------------------
The .ico file format is antique, and doesn't naturally handle images with more than 255 pixel dimension(s). Microsoft added a hack to allow one large PNG image by setting the dimensions of that image to 0.


Uncompressed Bitmaps
--------------------
While Windows 10 supports .ico files containing multiple PNGs, older versions of Windows do not. This simple program doesn't know how to understand PNG files, and only makes .ico files compatible with all versions of Windows.


MS ICO specifiation:
--------------------

|**block**	|**offset**	|**offset**	|**length**	|**description**				|
|-----------|-----------|-----------|-----------|-------------------------------|
|main header|	0		| 			|	2		|Reserved=0						|
|			|	2		|			|	2		|Image type: 1(.ICO) 2(.CUR)	|
|			|	4		|			|	2		|Number of images in container	|
|image head1|	6		|	0		|	1		|Pixel width					|
|			|	7		|	1		|	1		|Pixel height					|
|			|	8		|	2		|	1		|Color palette size or 0		|
|			|	9		|	3		|	1		|Reserved=0						|
|			|	A 		|	4		|	2		|Color planes=0 or 1			|
|			|	C 		|	6		|	2		|Bits per Pixel 				|
|			|	E 		|	8		|	4		|Image raw size					|
|			|	12		|	C 		|	4		|Offset of imageblock from BOF 	|
|image head2|	16		|	0		|	1		|Pixel width					|
|	...		|	...		|	...		|	...		|...							|
|imageblock1|	... 	|	...		|	...		|all image data goes here: 		|
|			|	... 	|	...		|	...		|	pngs included in whole 		|
|			|	... 	|	...		|	...		|	bmps missing first 14 bytes	|