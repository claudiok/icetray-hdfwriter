from icecube.load_pybindings import load_pybindings
from icecube import icetray, tableio

import sys
major,minor = sys.version_info[:2]
# pytables only runs on 2.4 anyhow, and this script uses decorators
if (major >= 2 and minor >= 4):
    import pytables_ext

# clean the local dictionary
del sys,major,minor

load_pybindings(__name__, __path__)

@icetray.traysegment
def I3HDFWriter(tray, name, Output=None, **kwargs):
	"""Tabulate data to an HDF5 file.

	Arguments:
		Output: Path to output file
		Keys: Key names of frame objects to tabulate
		Streams: Subevent streams to include
		(See I3TableWriter documentation for others)
	"""
	tabler = I3HDFTableService(Output)
	tray.AddModule(tableio.I3TableWriter, name, TableService=tabler,
	    **kwargs)

# clean the local dictionary
del icetray,tableio
