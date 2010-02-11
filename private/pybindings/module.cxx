/**
 * hdf-writer pybindings
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

#include <icetray/load_project.h>

namespace bp = boost::python;

#define REGISTER_THESE_THINGS \
   (I3TableRowDescription)(I3TableRow)(I3Converter)(I3TableService)     \
   (I3TableWriter)(I3TableTranscriber)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(hdf_writer)
{
  load_project("libhdf-writer", false);

  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
