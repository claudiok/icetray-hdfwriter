/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TableWriter.h 61608 2010-02-11 19:23:20Z jvansanten $
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
 */

#include <hdf-writer/internals/I3TableTranscriber.h>
#include <hdf-writer/internals/I3TableRowDescription.h>
#include <hdf-writer/internals/I3TableRow.h>

#include <dataclasses/physics/I3EventHeader.h>

I3TableTranscriber::I3TableTranscriber(I3TableServicePtr input, I3TableServicePtr output) 
    : inputService_(input), outputService_(output), nEvents_(0) {
        
        std::vector<std::string>::iterator it;
        std::vector<std::string> inputTables = inputService_->GetTableNames();
        std::map<std::string, I3TablePtr> inputTableMap;
        I3TablePtr table;
        unsigned int nEvents = 0;
        std::string nEvents_reference;
        unsigned int nrows = 0;
        for (it = inputTables.begin(); it != inputTables.end(); it++) {
            table = inputService_->GetTable(*it, I3TableRowDescriptionConstPtr());
            nrows = table->GetNumberOfRows();
            // if this is not a ragged table, then it should have exactly
            // one row for every event in the table 
            if (!table->GetDescription()->GetIsMultiRow()) {
                // in particular, every non-ragged table should
                // have the same number of rows
                if (nEvents != 0) {
                    if (nrows != nEvents) {
                        log_fatal("Input tables '%s' '%s' have different sizes (%u vs. %u)",
                            nEvents_reference.c_str(),table->GetName().c_str(),nEvents,nrows);
                    }
                } else {
                    nEvents = nrows;
                    nEvents_reference = table->GetName();
                }
            }
           
            inputTableMap[*it] = table;
        }
        
        if (nEvents == 0) {
            log_fatal("Input file contains no events. [FIXME: this could happen if there are no non-ragged tables.]");
        }
        
        nEvents_ = nEvents;
        
        std::map<std::string, I3TablePtr>::iterator t_it;
        I3TablePtr otable;
        std::string name;
        for (t_it = inputTableMap.begin(); t_it != inputTableMap.end(); t_it++) {
            name = t_it->first;
            table = t_it->second;
            log_trace("Creating output table '%s'",name.c_str());
            // otable = ConnectTable(name,*table->GetDescription());
            otable = outputService_->GetTable(name,table->GetDescription());
            if (!otable) log_fatal("Could not create output table '%s'",name.c_str());
            transcriptions_.push_back(std::make_pair(table,otable));
        }
        
}

void I3TableTranscriber::Execute() {
    Execute(nEvents_);
}

void I3TableTranscriber::Execute(unsigned int nframes) {
    // don't overrun the table
    if (nframes > nEvents_) nframes = nEvents_;
    unsigned int i;
    std::vector<std::pair<I3TablePtr,I3TablePtr> >::iterator pair_it;
    I3TablePtr in,out;
    I3EventHeaderPtr header;
    I3TableRowPtr rows;
    
    // loop over the given number of row-groups
    for (i = 0; i<nframes; i++) {
        log_trace("Transcribing event #%u",i+1);
        for (pair_it = transcriptions_.begin(); pair_it != transcriptions_.end(); pair_it++) {
            in = pair_it->first;
            out = pair_it->second;
            log_trace("Getting rows for '%s'",in->GetName().c_str());
            rows = boost::const_pointer_cast<I3TableRow>(in->GetRowForEvent(i));
            // don't transcribe padding rows
            // convention: if a block is padding, its first row
            // should always be marked as such
            if ((!rows) || (!rows->Get<bool>("exists"))) continue;
            header = I3EventHeaderPtr(new I3EventHeader());
            header->SetRunID(rows->Get<unsigned int>("Run"));
            header->SetEventID(rows->Get<unsigned int>("Event"));
            log_trace("R%u/E%u '%s': %u row(s)",
                header->GetRunID(),header->GetEventID(),in->GetName().c_str(),rows->GetNumberOfRows());
            
            out->AddRow(header,rows);
        }
    }
    
    Finish();
}

void I3TableTranscriber::Finish() {
    // disconnect from all tables
    std::vector<std::pair<I3TablePtr,I3TablePtr> >::iterator pair_it;
    for (pair_it = transcriptions_.begin(); pair_it != transcriptions_.end(); pair_it++) {
        DisconnectTable(pair_it->second);
    }

}

/******************************************************************************/

I3TablePtr I3TableTranscriber::ConnectTable(std::string tableName, 
                                       const I3TableRowDescription& description) {
    log_debug("Connecting to table %s", tableName.c_str());
    I3TableRowDescriptionPtr descPointer = 
        I3TableRowDescriptionPtr(new I3TableRowDescription(description));
    I3TablePtr table = outputService_->GetTable(tableName, descPointer);
    if (!table) {
        log_fatal("couldn't get table %s from the service", tableName.c_str());
    }
    table->SetConnectedToWriter(true);
    return table;
}

/******************************************************************************/

void I3TableTranscriber::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str());
    table->Align();
    table->SetConnectedToWriter(false);
    table.reset();
}

/******************************************************************************/


