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

#include "hdf-writer/internals/I3Table.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3TableRowDescription.h"
#include "hdf-writer/internals/I3TableService.h"

/******************************************************************************/

I3Table::I3Table(I3TableService& service, 
                 std::string name,
                 I3TableRowDescriptionConstPtr description) :
    service_(service),
    name_(name),
    description_(description),
    indexTable_(),
    nevents_(0),
    nrows_(0),
    connected_(false),
    tableCreated_(false) {
    // implementation pending
}

/******************************************************************************/

I3Table::~I3Table() {}; // Flush?

/******************************************************************************/

bool I3Table::IsConnectedToWriter() {
    return connected_;
}

/******************************************************************************/

void I3Table::SetConnectedToWriter(bool connected) {
    connected_ = connected;
}

/******************************************************************************/
        
I3TableRowDescriptionConstPtr I3Table::GetDescription() {
    return description_;
}

/******************************************************************************/

void I3Table::AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row) {
    /* TODO add error checking;
       in principle this can break if the padding rows are too large
       (e.g. creation of a huge table at the end of a processing chain)
       maybe replace this with a while(padding = ...) loop
       and hand out rows in chunks 
    */
    assert(row->GetDescription() == description_);
    I3TableRowConstPtr padding = 
        service_.GetPaddingRows(lastHeader_, header, description_);
    if (padding)
        WriteRows(padding);

    WriteRows(row);
    nevents_++;
    nrows_ += row->GetNumberOfRows();
    lastHeader_ = header;
}

/******************************************************************************/

unsigned int I3Table::GetNumberOfEvents() const {
   return nevents_; 
}

/******************************************************************************/

unsigned int I3Table::GetNumberOfRows() const {
    return nrows_;
}

/******************************************************************************/

std::string I3Table::GetName() const {
    return name_;
}

/******************************************************************************/
        
I3TableRowPtr I3Table::CreateRow(unsigned int nrows) {
    return I3TableRowPtr( new I3TableRow(description_, nrows) );
}

/******************************************************************************/
