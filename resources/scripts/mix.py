#!/usr/bin/env python

import re
from subprocess import Popen, PIPE, call

def h5ls(fname):
	objs = []
	for line in Popen(['h5ls', '-r', fname], stdout=PIPE).communicate()[0].split('\n'):
		fields = re.split('\s+', line)
		if len(fields) < 2:
			continue
		path, kind = fields[:2]
		if kind == 'Dataset':
			size = int(fields[-1][1:-1].split('/')[0])
		else:
			size = -1
		objs.append((path, size))
	return objs
	
def h5copy(from_file, to_file, path):
	call(['h5copy', '--flag=shallow', '-i', from_file, '-o', to_file, '-s', path, '-d', path])

def h5inherit(infiles, outfile):
	tableset = set([i[0] for i in h5ls(outfile)])
	copied = []
	for fn in infiles:
		for path, size in h5ls(fn):
			if not path in tableset:
				copied.append((fn, path, size))
				h5copy(fn, outfile, path)
				tableset.add(path)
	return copied
		
if __name__ == "__main__":
	
	from optparse import OptionParser
	import sys, os
	
	parser = OptionParser(description='Copy tables into an existing HDF5 file. If a table with the same name exists in multiple files, it will be take from the last one specified.',
	    usage='%prog: [OPTIONS] FILES')
	parser.add_option('-o', dest='output', default=None, help='If specified, tables will be copied into OUTPUT. Otherwise, new tables will be added to the last file in FILES.')
	
	opts, infiles = parser.parse_args()
	
	if len(infiles) < 2:
		parser.error('Specifiy at least two input files.')
		
	for fn in infiles:
		if not os.path.exists(fn):
			parser.error('Input file %s does not exist!' % fn)
	
	if opts.output is None:
		copied = h5inherit(infiles[:-1], infiles[-1])
	else:
		if not os.path.exists(opts.output):
			call(['cp', infiles[-1], opts.output])
		copied = h5inherit(infiles[:-1], opts.output)
	
	if len(copied) == 0:
		print 'All tables present in output; nothing copied.'
	else:
		print 'Copied %d datasets:' % len(copied)
		for fn, path, size in copied:
			print '    %s (%d rows) from %s' % (path, size, fn)
	