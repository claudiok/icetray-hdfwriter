from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

import sys
major,minor = sys.version_info[:2]
# pytables only runs on 2.4 anyhow, and this script uses decorators
if (major >= 2 and minor >= 4):
    import pytables_ext
    
del sys,major,minor
