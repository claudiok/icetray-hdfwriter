from icecube.load_pybindings import load_pybindings
import icecube.tableio # pull in direct dependencies

import sys
major,minor = sys.version_info[:2]
# pytables only runs on 2.4 anyhow, and this script uses decorators
if (major >= 2 and minor >= 4):
    import pytables_ext

# clean the local dictionary
del sys,major,minor,icecube

load_pybindings(__name__, __path__)


