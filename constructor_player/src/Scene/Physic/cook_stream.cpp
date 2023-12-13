#include "./cook_stream.h"

cCookStreamBuffer::cCookStreamBuffer() : _currentSize( 0 ), _maxSize( 0 ), _pWriteData( NULL ), _pReadBuffer( NULL )
{
}

cCookStreamBuffer::~cCookStreamBuffer()
{
	NX_DELETE_ARRAY( _pWriteData );

	// !!! We don't own the data => no delete [_pReadBuffer]
}

/*void cCookStreamBuffer::clear()
{
	currentSize = 0;
}*/

NxStream& cCookStreamBuffer::storeByte(NxU8 b)
{
	storeBuffer( &b, sizeof(NxU8) );
	return *this;
}
NxStream& cCookStreamBuffer::storeWord(NxU16 w)
{
	storeBuffer( &w, sizeof(NxU16) );
	return *this;
}
NxStream& cCookStreamBuffer::storeDword(NxU32 d)
{
	storeBuffer( &d, sizeof(NxU32) );
	return *this;
}
NxStream& cCookStreamBuffer::storeFloat(NxReal f)
{
	storeBuffer( &f, sizeof(NxReal) );
	return *this;
}
NxStream& cCookStreamBuffer::storeDouble(NxF64 f)
{
	storeBuffer( &f, sizeof(NxF64) );
	return *this;
}
NxStream& cCookStreamBuffer::storeBuffer( const void* buffer, NxU32 size )
{
	NxU32 expectedSize = _currentSize + size;
	if( expectedSize > _maxSize )
	{
		_maxSize = expectedSize + 4096;

		NxU8* newData = new NxU8[_maxSize];
		NX_ASSERT( newData != NULL );

		if( _pWriteData )
		{
			memcpy( newData, _pWriteData, _currentSize );
			delete[] _pWriteData;
		}
		_pWriteData = newData;
	}

	memcpy( _pWriteData + _currentSize, buffer, size );
	_currentSize += size;
	return *this;
}


// read :)
NxU8 cCookStreamBuffer::readByte() const
{
	NxU8 b;
	memcpy( &b, _pReadBuffer, sizeof(NxU8) );
	_pReadBuffer += sizeof(NxU8);
	return b;
}

NxU16 cCookStreamBuffer::readWord() const
{
	NxU16 w;
	memcpy( &w, _pReadBuffer, sizeof(NxU16) );
	_pReadBuffer += sizeof(NxU16);
	return w;
}

NxU32 cCookStreamBuffer::readDword() const
{
	NxU32 d;
	memcpy( &d, _pReadBuffer, sizeof(NxU32) );
	_pReadBuffer += sizeof(NxU32);
	return d;
}

float cCookStreamBuffer::readFloat() const
{
	float f;
	memcpy( &f, _pReadBuffer, sizeof(float) );
	_pReadBuffer += sizeof(float);
	return f;
}

double cCookStreamBuffer::readDouble() const
{
	double f;
	memcpy( &f, _pReadBuffer, sizeof(double) );
	_pReadBuffer += sizeof(double);
	return f;
}

void cCookStreamBuffer::readBuffer(void* dest, NxU32 size) const
{
	memcpy( dest, _pReadBuffer, size );
	_pReadBuffer += size;
}