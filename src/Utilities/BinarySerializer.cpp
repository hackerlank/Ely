/**
 * \file BinarySerializer.cpp
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#include "oge/serialisation/OgeBinarySerialiser.h"
#include "oge/math/OgeVector3.h"
#include "oge/math/OgeQuaternion.h"
#include "oge/datastreams/OgeDataStream.h"

namespace utils
{
    //-------------------------------------------------------------------------
    int BinarySerialiser::readInt()
    {
        int n = 0;
        mDataStream->read((char*)&n, sizeof(int));
        return n;
    }
    //-------------------------------------------------------------------------
    float BinarySerialiser::readFloat()
    {
        float f = 0;
        mDataStream->read((char*)&f, sizeof(float));
        return f;
    }
    //-------------------------------------------------------------------------
    std::string BinarySerialiser::readString()
    {
        int nLength = readInt();
        char txt[512]; // TODO see writeString()
        mDataStream->read(txt, nLength);
        return std::string(txt);
    }
    //-------------------------------------------------------------------------
    Vector3 BinarySerialiser::readVector3()
    {
        Vector3 v;
        v.x = readFloat();
        v.y = readFloat();
        v.z = readFloat();
        return v;
    }
    //-------------------------------------------------------------------------
    Quaternion BinarySerialiser::readQuaternion()
    {
        Quaternion q;
        q.w = readFloat();
        q.x = readFloat();
        q.y = readFloat();
        q.z = readFloat();
        return q;
    }
    //-------------------------------------------------------------------------
    void* BinarySerialiser::readPointer()
    {
        void* p;
        mDataStream->read((char*)&p, sizeof(void*));
        return p;
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writeInt(int n)
    {
        mDataStream->write((char*)&n, sizeof(int));
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writeFloat(float f)
    {
        mDataStream->write((char*)&f, sizeof(float));
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writeString(const std::string & s)
    {
        int nLength = (int) (s.length()+1);

        // TODO See readString() to avoid new/deleting char array all the time 
        // we need to parse a long text into chunks. My quick try needed more testing.
        if (nLength > 512)
            nLength = 512; // for now I trunk it to 512 chars

        writeInt (nLength);
        mDataStream->write((const char*) (s.c_str()), nLength);
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writeVector3(const Vector3& v)
    {
        writeFloat( v.x );
        writeFloat( v.y );
        writeFloat( v.z );
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writeQuaternion(const Quaternion& q)
    {
        writeFloat( q.w );
        writeFloat( q.x );
        writeFloat( q.y );
        writeFloat( q.z );
    }
    //-------------------------------------------------------------------------
    void BinarySerialiser::writePointer(void *p)
    {
        mDataStream->write((char*)&p, sizeof(void*));
    }
    //-------------------------------------------------------------------------
}
