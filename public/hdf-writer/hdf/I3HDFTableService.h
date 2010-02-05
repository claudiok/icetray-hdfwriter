/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3HDFTABLESERVICE_H_INCLUDED
#define I3HDFTABLESERVICE_H_INCLUDED

#include "hdf-writer/internals/I3TableService.h"

// hdf5 includes
#include "H5TA.h"

class I3HDFTableService : public I3TableService {
    public:
        I3HDFTableService(const std::string& filename, int compress=1);
        virtual ~I3HDFTableService();


    protected:
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description);
        virtual void CloseFile();
    private:
        hid_t fileId_;
        std::string filename_;
        int compress_;
};
#endif
