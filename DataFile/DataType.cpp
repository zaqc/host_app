#include "DataType.h"

#ifdef WINDOWS_OS
HINSTANCE DataTypeConverter::m_hResInstance = 0;
#endif

DataType& operator++(DataType &aDataType) 
{
	aDataType = static_cast<DataType>( static_cast<int>(aDataType) + 1 );
	if(aDataType == DataType::END_OF_DATATYPE)
		aDataType = DataType::dtNONE;
	return aDataType;
}
