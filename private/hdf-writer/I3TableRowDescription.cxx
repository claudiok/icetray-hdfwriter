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

#include "hdf-writer/internals/I3TableRowDescription.h"
#include <numeric>

/******************************************************************************/

hid_t HDFTypeConv::GetType(float x)          { return H5T_NATIVE_FLOAT; }
hid_t HDFTypeConv::GetType(double x)         { return H5T_NATIVE_DOUBLE; }
hid_t HDFTypeConv::GetType(long double x)    { return H5T_NATIVE_LDOUBLE; }
hid_t HDFTypeConv::GetType(char x)           { return H5T_NATIVE_CHAR; }
hid_t HDFTypeConv::GetType(unsigned char x)  { return H5T_NATIVE_UCHAR; }
hid_t HDFTypeConv::GetType(signed char x)    { return H5T_NATIVE_SCHAR; }
hid_t HDFTypeConv::GetType(short x)          { return H5T_NATIVE_SHORT; }
hid_t HDFTypeConv::GetType(unsigned short x) { return H5T_NATIVE_USHORT; }
hid_t HDFTypeConv::GetType(int x)            { return H5T_NATIVE_INT; }
hid_t HDFTypeConv::GetType(unsigned x)       { return H5T_NATIVE_UINT; }
hid_t HDFTypeConv::GetType(long x)           { return H5T_NATIVE_LONG; }
hid_t HDFTypeConv::GetType(unsigned long x)  { return H5T_NATIVE_ULONG; }
hid_t HDFTypeConv::GetType(long long x)      { return H5T_NATIVE_LLONG; }
hid_t HDFTypeConv::GetType(bool x)           { return H5T_NATIVE_HBOOL; }

//FIXME: there's got to be a better way to do this
char PyTypeConv::GetTypeCode(hid_t hdf_type) {
	// grab the native datatype of the atom
	if ( H5Tget_class(hdf_type) == H5T_ENUM ) {
		hdf_type = H5Tget_super(hdf_type);
	}
   hid_t n = H5Tget_native_type(hdf_type,H5T_DIR_ASCEND);
   char code = 0;
   if        ( H5Tequal(n,H5T_NATIVE_FLOAT)  ) {
          code = 'f'; // python double       
   } else if ( H5Tequal(n,H5T_NATIVE_DOUBLE) ) {
          code = 'd'; // python double       
   } else if ( H5Tequal(n,H5T_NATIVE_CHAR)   ) {
          code = 'c'; // python character    
   } else if ( H5Tequal(n,H5T_NATIVE_UCHAR)  ) {
          code = 'B'; // python int          
   } else if ( H5Tequal(n,H5T_NATIVE_SCHAR)  ) {
          code = 'b'; // python int          
   } else if ( H5Tequal(n,H5T_NATIVE_SHORT)  ) {
          code = 'h'; // python int          
   } else if ( H5Tequal(n,H5T_NATIVE_USHORT) ) {
          code = 'H'; // python int          
   } else if ( H5Tequal(n,H5T_NATIVE_INT)    ) {
          code = 'i'; // python int          
   } else if ( H5Tequal(n,H5T_NATIVE_UINT)   ) {
          code = 'I'; // python long         
   } else if ( H5Tequal(n,H5T_NATIVE_LONG)   ) {
          code = 'l'; // python long         
   } else if ( H5Tequal(n,H5T_NATIVE_ULONG)  ) {
          code = 'L'; // python long
   } else if ( H5Tequal(n,H5T_NATIVE_HBOOL)  ) {
          code = 'o'; // bool
   }
	H5Tclose(n);
   return code;
}

/******************************************************************************/

I3TableRowDescription::I3TableRowDescription() {}

/******************************************************************************/

I3TableRowDescription::~I3TableRowDescription() {}

/******************************************************************************/

const std::vector<std::string>& I3TableRowDescription::GetFieldNames() const {
    return fieldNames_;
}

const std::vector<hid_t>&  I3TableRowDescription::GetFieldHdfTypes() const {
    return fieldHdfTypes_;
}

const std::vector<char>&  I3TableRowDescription::GetFieldTypeCodes() const {
    return fieldTypeCodes_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldTypeSizes() const {
    return fieldTypeSizes_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldArrayLengths() const {
    return fieldArrayLengths_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldByteOffsets() const {
    return fieldByteOffsets_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldChunkOffsets() const {
    return fieldChunkOffsets_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldUnits() const {
    return fieldUnits_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldDocStrings() const {
    return fieldDocStrings_;
}

/******************************************************************************/
        
unsigned int I3TableRowDescription::GetFieldColumn(const std::string& fieldName) const {
    std::map<std::string,size_t>::const_iterator iter;
    iter = fieldNameToIndex_.find(fieldName);
    if (iter != fieldNameToIndex_.end()) {
        return iter->second;
    }
    else {
        return -1;
    }
}

/******************************************************************************/

bool I3TableRowDescription::CanBeFilledInto(I3TableRowDescriptionConstPtr other) const {
    int nfields = GetNumberOfFields();
    bool compatible = true;
    for (int i; i < nfields; ++i) {
        if ( (fieldNames_.at(i) != other->GetFieldNames().at(i) ) ||
             (fieldHdfTypes_.at(i) != other->GetFieldHdfTypes().at(i) ) ||
             (fieldTypeSizes_.at(i) != other->GetFieldTypeSizes().at(i) ) ) // redundant...
            compatible = false;
    }

    return compatible;
}

/******************************************************************************/

void I3TableRowDescription::AddField(const std::string& name, hid_t hdfType, 
                                     size_t typeSize, const std::string& unit,
                                     const std::string& doc, size_t arrayLength) {

    log_trace("adding field %s with type %d of size %d", name.c_str(), hdfType, (int)typeSize);
    size_t chunkOffset=0;
    size_t byteOffset=0;
    if (fieldChunkOffsets_.size() > 0) {
        chunkOffset = GetTotalChunkSize();
        byteOffset = GetTotalByteSize();
    }    
    size_t nfields = fieldNameToIndex_.size();
    fieldNames_.push_back(name);
    fieldNameToIndex_[name] = nfields;
    if (arrayLength == 1)
        fieldHdfTypes_.push_back(hdfType);
    else {
        hsize_t rank = 1; 
        std::vector<hsize_t> dims(1, arrayLength);
        hid_t array_tid = H5Tarray_create(hdfType, rank, &dims.front(), NULL);
        fieldHdfTypes_.push_back(array_tid);
    }
    char typeCode = PyTypeConv::GetTypeCode(hdfType);
	if (typeCode == 0) log_error("No type code for field '%s'",name.c_str());
    fieldTypeCodes_.push_back(typeCode);
    fieldTypeSizes_.push_back(typeSize);
    fieldArrayLengths_.push_back(arrayLength);
    fieldChunkOffsets_.push_back(chunkOffset);
    fieldByteOffsets_.push_back(byteOffset);
    fieldUnits_.push_back(unit);
    fieldDocStrings_.push_back(doc);
}

/******************************************************************************/
        
size_t I3TableRowDescription::GetTotalChunkSize() const {
    // TODO catch no defined field case
    return fieldChunkOffsets_.back() + fieldArrayLengths_.back();
}

size_t I3TableRowDescription::GetTotalByteSize() const {
    // TODO catch no defined field case
    return I3MEMORYCHUNK_SIZE * GetTotalChunkSize();
}


/******************************************************************************/
        
unsigned int I3TableRowDescription::GetNumberOfFields() const {
    return fieldNames_.size(); 
}

/******************************************************************************/

I3TableRowDescription operator|(const I3TableRowDescription& lhs, 
                                const I3TableRowDescription& rhs) {
    I3TableRowDescription newlhs = I3TableRowDescription(lhs);
    int nfields = rhs.GetNumberOfFields();
    for (int i=0; i < nfields; i++) {
        size_t byteOffset=0;
        size_t chunkOffset=0;
        if (newlhs.GetNumberOfFields() > 0)
            byteOffset = newlhs.GetTotalByteSize();
            chunkOffset = newlhs.GetTotalChunkSize();

        std::string fieldName = rhs.fieldNames_.at(i);
        
        // values that are just copied:
        newlhs.fieldNames_.push_back( fieldName );
        newlhs.fieldTypeSizes_.push_back( rhs.fieldTypeSizes_.at(i) );
        newlhs.fieldArrayLengths_.push_back( rhs.fieldArrayLengths_.at(i) );
        newlhs.fieldUnits_.push_back( rhs.fieldUnits_.at(i) );
        newlhs.fieldDocStrings_.push_back( rhs.fieldDocStrings_.at(i) );
        
        // values that have to be recalculated:
        newlhs.fieldNameToIndex_[fieldName] = newlhs.fieldNameToIndex_.size();
        newlhs.fieldByteOffsets_.push_back(byteOffset);
        newlhs.fieldChunkOffsets_.push_back(chunkOffset);
    }
    return newlhs;
}
        


