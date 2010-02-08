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

#ifndef	I3_TABLEROWDESCRIPTION_H_INCLUDED
#define I3_TABLEROWDESCRIPTION_H_INCLUDED

// icetray includes
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

// hdf5 includes
#include "H5TA.h"

#include <string>
#include <vector>

// boost::assign for less tedious enums
#include <boost/assign/list_inserter.hpp>
#include <boost/preprocessor/seq/transform.hpp>

#include "hdf-writer/internals/I3MemoryChunk.h"

/*****************************************************************************/

#define WRAP_ELEMENTS(R,DATA,ELEM) BOOST_PP_STRINGIZE(ELEM),DATA::ELEM
#define MAKE_ENUM_VECTOR(VECTORNAME,CLASS,ENUM_TYPE,FIELDS)		\
    std::vector< std::pair<std::string,CLASS::ENUM_TYPE> > VECTORNAME ;	\
    boost::assign::push_back(VECTORNAME) BOOST_PP_SEQ_TRANSFORM(WRAP_ELEMENTS,CLASS,FIELDS); \

/*****************************************************************************/

/*****************************************************************************/


class I3TableRowDescription {


public:
    I3TableRowDescription();
    virtual ~I3TableRowDescription();

    // TODO logger
        
    /* basic AddField */
    void AddField(const std::string& name, hid_t hdfType, char typeCode,
		  size_t typeSize, const std::string& unit,
		  const std::string& doc,
		  size_t arrayLength);

    /* convenience AddField */
    template<class T>
    void AddField(const std::string& name, 
		  const std::string& unit,
		  const std::string& doc, 
		  size_t arrayLength=1) 
    {
	AddField(name, hdf_type(T()), py_code(T()), sizeof(T), 
		 unit, doc, arrayLength);
    }

    /* convenience AddEnumField - create hdf type and call AddField */
    template<typename enum_type>
    void AddEnumField(const std::string& name, 
		      const std::vector<std::pair<std::string,enum_type> > &elements,
		      const std::string& unit,
		      const std::string& doc,
		      size_t arrayLength=1) {
            
	// TODO check enumsize <= DATAENTRYSIZE
	typename std::vector<std::pair<std::string,enum_type> >::const_iterator it;
	enum_type enum_instance;
	hid_t enum_tid = H5Tcreate(H5T_ENUM, sizeof(enum_type));
	for (it = elements.begin(); it != elements.end(); ++it) {
	    H5Tenum_insert(enum_tid, it->first.c_str(),
			   (enum_instance=it->second, &enum_instance));
	}
	char typeCode = 'i';
	AddField(name, enum_tid, typeCode, sizeof(enum_type), unit, doc, arrayLength);
    }

    bool CanBeFilledInto(shared_ptr<const I3TableRowDescription> other) const;
        
    // getter and setter - remove them? no real encapsulation anyway
    const std::vector<std::string>& GetFieldNames() const;
    const std::vector<hid_t>&  GetFieldHdfTypes() const;
    const std::vector<char>& GetFieldTypeCodes() const;
    // units of bytes
    const std::vector<size_t>& GetFieldTypeSizes() const;
    const std::vector<size_t>& GetFieldByteOffsets() const;
    const std::vector<size_t>& GetFieldChunkOffsets() const;
    const std::vector<size_t>& GetFieldArrayLengths() const;
    const std::vector<std::string>& GetFieldUnits() const;
    const std::vector<std::string>& GetFieldDocStrings() const;

    unsigned int GetFieldColumn(const std::string& fieldName) const;
    size_t GetTotalByteSize() const;
    size_t GetTotalChunkSize() const;
    unsigned int GetNumberOfFields() const;

    bool operator==(shared_ptr<const I3TableRowDescription> other) const;
private:
    std::vector<std::string> fieldNames_;
    std::map<std::string, size_t> fieldNameToIndex_;
    std::vector<hid_t> fieldHdfTypes_;
    std::vector<char> fieldTypeCodes_;
    std::vector<size_t> fieldTypeSizes_;
    std::vector<size_t> fieldArrayLengths_;
    std::vector<size_t> fieldByteOffsets_;
    std::vector<size_t> fieldChunkOffsets_;
    std::vector<std::string> fieldUnits_;
    std::vector<std::string> fieldDocStrings_;
        
#define CODEMAP_DEF(TYPE, CODE, TYPECODE)				\
    const static inline hid_t hdf_type(TYPE) { return  CODE; }		\
    const static inline char  py_code(TYPE)  { return TYPECODE; }
    
    CODEMAP_DEF(float, H5T_NATIVE_FLOAT, 'f');
    CODEMAP_DEF(double, H5T_NATIVE_DOUBLE, 'd');
    CODEMAP_DEF(long double, H5T_NATIVE_LDOUBLE, '\0');
    CODEMAP_DEF(char, H5T_NATIVE_CHAR, 'c');
    CODEMAP_DEF(unsigned char, H5T_NATIVE_UCHAR, 'B');
    CODEMAP_DEF(signed char, H5T_NATIVE_SCHAR, 'b');
    CODEMAP_DEF(short, H5T_NATIVE_SHORT, 'h');
    CODEMAP_DEF(unsigned short, H5T_NATIVE_USHORT, 'H');
    CODEMAP_DEF(int, H5T_NATIVE_INT, 'i');
    CODEMAP_DEF(unsigned int, H5T_NATIVE_UINT, 'I');
    CODEMAP_DEF(long int, H5T_NATIVE_LONG, 'l');
    CODEMAP_DEF(unsigned long int, H5T_NATIVE_ULONG, 'L');
    CODEMAP_DEF(bool, H5T_NATIVE_HBOOL, 'o');

  #undef CODEMAP_DEF

    friend I3TableRowDescription operator|(const I3TableRowDescription& lhs, const I3TableRowDescription& rhs);



};

I3_POINTER_TYPEDEFS( I3TableRowDescription );

#endif
