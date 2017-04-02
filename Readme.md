Surface evolver api
============================
## Installation
The code depends on third-party applications:
- `voro++` for tessellation


To install `voro++`:
```
sudo ./install_voro++.sh
```
If that doesn't work, you will need to download it from
http://math.lbl.gov/voro++/download/ and install it manually.

## Build
	cmake .
	make

## Run
	SE_api -i foam_simple.geo
	SE_api -g cubic

## Description
	-i input.geo
		Load file in geo format
	-g random/cubic/hexab
		Generate structure by using voro++ tesselation
	-n 9
		Specify number of cells to generate
			for g:
				random = number of cells
				cubic = number of cells in one direction
				hexab =  even number of cells in one direction
	-o outputFile
		Specify name for output outputFile.fe and outputFile.cmd
	-c file1.cmd,file2.cmd,file3.cmd
		Comma separated list of cmd files which will be loaded in addition when surface evolver is run with output cmd file.

## Example
	SE_api -i foam_simple.geo
	evolver foam.fe
