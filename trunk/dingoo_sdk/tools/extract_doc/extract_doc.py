#!/usr/bin/python

import sys

block = False

for fh in sys.argv[1:]:
	f = open(fh)
	data = []

	for line in f:
		if "/**" in line:
			block = True
		elif "*/" in line:
			block = False
		elif block:
			data.append( line )
		
	if len(data) > 0:
		outf = open(fh+".wiki", "w")
		for line in data:
			outf.write( line )
			
		outf.close()
	
	f.close()
