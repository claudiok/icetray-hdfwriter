/**
 * I3TableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include <hdfwriter/I3HDFTableService.h>

namespace bp = boost::python;


void register_I3HDFTableService() {
   bp::class_<I3HDFTableService, 
      boost::shared_ptr<I3HDFTableService>, bp::bases<I3TableService> >
      ("I3HDFTableService", bp::init<const std::string>(bp::args("filename")))
      .def(bp::init<const std::string&,int>(bp::args("filename","compression_level")))
      .def(bp::init<const std::string&,int,char>(bp::args("filename","compression_level","mode")))

		;
}
