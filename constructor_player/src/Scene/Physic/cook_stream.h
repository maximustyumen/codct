// -----------------------------------------------
// vip_open		[86open@gmail.com]		20.05.2009
// -----------------------------------------------

#ifndef PROTO_OPEN_COOK_STREAM_H
#define PROTO_OPEN_COOK_STREAM_H

#include <NxSimpleTypes.h>
#include <Nxf.h>
#include <NxStream.h>

// буфер-поток "подготовки" данных
class cCookStreamBuffer: public NxStream
{
public:
	cCookStreamBuffer();
	virtual ~cCookStreamBuffer();

	// write buffer
public:

	virtual NxStream& storeByte( NxU8 b );
	virtual NxStream& storeWord( NxU16 w );
	virtual NxStream& storeDword( NxU32 d );
	virtual NxStream& storeFloat( NxReal f );
	virtual NxStream& storeDouble( NxF64 f );
	virtual NxStream& storeBuffer( const void* buffer, NxU32 size );
private:
	NxU32 _currentSize;
	NxU32 _maxSize;
	NxU8* _pWriteData;

	// read buffer
public:
	void toRead()	{	_pReadBuffer = _pWriteData;	}
	//void setReadBuffer( NxU8* pReadBuffer )	{	_pReadBuffer = pReadBuffer;	}
	virtual NxU8 readByte()  const;
	virtual NxU16 readWord()  const;
	virtual NxU32 readDword()  const;
	virtual float readFloat()  const;
	virtual double readDouble()  const;
	virtual void readBuffer(void* buffer, NxU32 size)  const;
private:
	mutable const NxU8* _pReadBuffer;
};

#endif
