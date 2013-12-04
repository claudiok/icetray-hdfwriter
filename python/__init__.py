from icecube.load_pybindings import load_pybindings
from icecube import icetray, tableio, dataio

import sys
major,minor = sys.version_info[:2]
# pytables only runs on 2.4 anyhow, and this script uses decorators
if (major, minor) >= (2, 4):
    from icecube.hdfwriter import pytables_ext

# clean the local dictionary
del sys,major,minor

load_pybindings(__name__, __path__)

@icetray.traysegment_inherit(tableio.I3TableWriter,
    removeopts=('TableService',))
def I3HDFWriter(tray, name, Output=None, CompressionLevel=6, **kwargs):
	"""Tabulate data to an HDF5 file.

	:param Output: Path to output file
	:param CompressionLevel: gzip compression to apply to each table
	"""
	
	if Output is None:
		raise ValueError("You must supply an output file name!")
	
	# Ready file for staging out if so configured
	if 'I3FileStager' in tray.context:
		stager = tray.context['I3FileStager']
		Output = stager.GetWriteablePath(Output)
	
	tabler = I3HDFTableService(Output, CompressionLevel)
	tray.AddModule(tableio.I3TableWriter, name, TableService=tabler,
	    **kwargs)

# clean the local dictionary
del icetray
