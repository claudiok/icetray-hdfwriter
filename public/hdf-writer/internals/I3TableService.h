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

#ifndef	I3TABLESERVICE_H_INCLUDED
#define I3TABLESERVICE_H_INCLUDED

#include <string>

#include "icetray/IcetrayFwd.h"

#include "dataclasses/physics/I3EventHeader.h"
#include "hdf-writer/internals/I3Table.h"

I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3Table);
I3_FORWARD_DECLARATION(I3Converter);

class I3TableService {
    public:
        I3TableService();

        virtual I3TablePtr GetTable(std::string tableName, 
                            I3TableRowDescriptionConstPtr description);

        I3TableRowConstPtr GetPaddingRows(I3EventHeaderConstPtr lastHeader,
                                          I3EventHeaderConstPtr newHeader,
                                          I3TableRowDescriptionConstPtr description_);

        void Finish();

    protected:
        // to be overridden by implementation
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description) = 0;
        virtual void CloseFile() = 0;

    private:
        bool EventHeadersEqual(const I3EventHeader& header1,
                               const I3EventHeader& header2);
        
        std::map<std::string, I3TablePtr> tables_;
        std::vector<I3EventHeaderConstPtr> eventHeaderCache_;
        I3ConverterPtr ticConverter_;

};

I3_POINTER_TYPEDEFS( I3TableService );



#endif
