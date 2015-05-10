## Microcontroller Interconnect Protocol version 1.0

This MIN repository includes the specification, a standard C API and
reference implementations for C and Python. See the Wiki for further
details:

http://github.com/min-protocol/min/wiki

This version has been slightly adapted for Arduino Serial communicationa and to look a bit like
CmdMessenger https://github.com/thijse/Arduino-CmdMessenger

File structure:

	firmware/				Embedded C code
		min/			
			layer1.c		MIN 1.0 reference implementation
			min.h			API definition
		main.c				Test program
		layer2_helper.h			Application layer for example program
		layer2_helper.c
			
	host/					Python code
		min.py				MIN 1.0 reference implementation (in Frame and SerialHandler classes)

All software and documentation available under the terms of the MIT License:

	The MIT License (MIT)
	
	Copyright (c) 2014-2015 JK Energy Ltd.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

