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


#include "hdfwriter/I3HDFTableService.h"
#include "hdfwriter/I3HDFTable.h"
#include "tableio/internals/I3TableRowDescription.h"

#include "H5Tpublic.h"

#if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
	#include "hdf5_hl.h"
#else
	#include "H5TA.h"
#endif

#include "H5Gpublic.h"
#include "H5Fpublic.h"

/******************************************************************************/

I3HDFTableService::I3HDFTableService(const std::string& filename, int compress, char mode) :
    I3TableService(),
    filename_(filename),
    compress_(compress),
    fileOpen_(false)
    // tables_()
    {
    if ( mode == 'w') { 
    fileId_ =  H5Fcreate(filename_.c_str(),
                         H5F_ACC_TRUNC, // truncate file if it exits
                         H5P_DEFAULT,   // default meta data creation 
                                        // - standard setting
                         H5P_DEFAULT);  // file access property 
                                        // - standard setting
    fileOpen_ = true;
    rootGroupId_ = H5Gopen(fileId_,"/");
    indexGroupId_ = H5Gcreate(rootGroupId_,"__I3Index__",1024);
   } else if ( mode == 'r' ) {
      fileId_ = H5Fopen(filename_.c_str(),
                        H5F_ACC_RDONLY,
                        H5P_DEFAULT );
      if (fileId_ < 0) {
         log_fatal("Could not open HDF file '%s'",filename_.c_str());
      }
      fileOpen_ = true;
      rootGroupId_ = H5Gopen(fileId_,"/");
      indexGroupId_ = H5Gopen(rootGroupId_,"__I3Index__");
      if (indexGroupId_ < 0) // create the group if it doesn't exist
         indexGroupId_ = H5Gcreate(rootGroupId_,"__I3Index__",1024);
      
      FindTables();
   } else {
      log_fatal("Unsupported file mode '%c'",mode);
   }
}

/******************************************************************************/

// A callback for browsing HDF nodes
herr_t GatherTableNames(hid_t group_id, const char* member_name, void* target) {
    H5G_stat_t object_info;
    H5Gget_objinfo(group_id, member_name, false, &object_info );
    // only gather datasets, not groups, links, or types
    if (object_info.type == H5G_DATASET) {
        reinterpret_cast<std::vector<std::string>*>(target)->push_back(std::string(member_name));
    }
    return 0;
}

/******************************************************************************/

// Gather tables hanging from the root note
void I3HDFTableService::FindTables() {

   // get my pretties from the root group
   std::vector<std::string> tableNames;
   H5Giterate(fileId_,"/",NULL,&GatherTableNames,&tableNames); 
   std::vector<std::string>::iterator t_it;
   std::string name;
   
   I3TablePtr index_table = I3TablePtr();
   
   for (t_it = tableNames.begin(); t_it != tableNames.end();) {
      hsize_t nfields,nrecords;
      name = "";
      name += *t_it;
      if (H5TBget_table_info(fileId_,name.c_str(),&nfields,&nrecords) < 0) {
         // erase returns iterator to following element
         t_it = tableNames.erase(t_it);
         log_error("Whups! Couldn't read dataset at %s",name.c_str());
         continue;
      }  
      
      // get the index table if it exists
      if (H5TBget_table_info(indexGroupId_,name.c_str(),&nfields,&nrecords) >= 0) {
         index_table = I3TablePtr(new I3HDFTable(*this,*t_it, indexGroupId_));
      } else {
         index_table = I3TablePtr();
         log_trace("(%s) no index table found.",t_it->c_str());
      }
      
      // it's a table, read in the description
      tables_[*t_it] = I3TablePtr(new I3HDFTable(*this,*t_it, fileId_, index_table));
      
      t_it++;
      
   }
}

/******************************************************************************/

I3HDFTableService::~I3HDFTableService() {};

/******************************************************************************/


I3TablePtr I3HDFTableService::CreateTable(const std::string& tableName, 
                              I3TableRowDescriptionConstPtr description) {
    
    I3TableRowDescriptionConstPtr index_desc = GetIndexDescription();
    I3TablePtr index_table(new I3HDFTable(*this, tableName, 
                                          index_desc, indexGroupId_, compress_));
    I3TablePtr table(new I3HDFTable(*this, tableName, 
                                    description, fileId_, compress_, index_table));
    return table;
};


void I3HDFTableService::CloseFile() {
    log_warn("Closing '%s'. Did I want to do some sanity checks first?",filename_.c_str());
    if (fileOpen_) {
        std::map<std::string, I3TablePtr>::iterator it;
        boost::shared_ptr<I3HDFTable> table;
        for (it = tables_.begin(); it != tables_.end(); it++) {
            // table = boost::dynamic_pointer_cast<I3HDFTable>(it->second);
            // table->Flush();
            it->second->Flush();
        }
        H5Fclose(fileId_);
        fileOpen_ = false;
    }
    else {
        log_warn("Additional call to CloseFile(). Maybe this should be prevented by the writer!");
    }
};
