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

#include "tableio/I3TableService.h"

// hdf5 includes
#include "H5Ipublic.h"

class I3HDFTableService : public I3TableService {
    public:
        I3HDFTableService(const std::string& filename, int compress=1, char mode='w');
        virtual ~I3HDFTableService();

    protected:
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description);
        virtual void CloseFile();

    private:
        
        void FindTables();

        hid_t fileId_;
        hid_t rootGroupId_;
        hid_t indexGroupId_;
        std::string filename_;
        int compress_;
        bool fileOpen_;

    SET_LOGGER("I3HDFTableService");
};


#endif
