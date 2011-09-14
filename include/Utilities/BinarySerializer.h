/**
 * \file BinarySerializer.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef __OGE_BINARYSERIALISER_H__
#define __OGE_BINARYSERIALISER_H__

namespace oge
{
    /** 
     * This class serialises classes (that implement the Serialisable interface)
     * in binary.
     *
     * @todo UTF8 string must be serialisable
     * @todo Some packing can be done (Vector, etc)
     *
     * @author Steven 'lazalong' Gay
     */
    class BinarySerialiser : public Serialiser
    {
    public:
        BinarySerialiser(DataStream* dataStream) : Serialiser(dataStream)
        {
        }

        // Read methods (must have a write equivalent)
        virtual int readInt();
        virtual float readFloat();
        virtual String readString();
        virtual Vector3 readVector3();
        virtual Quaternion readQuaternion();
        virtual void* readPointer();

        // Write methods (must have a read equivalent)
        virtual void writeInt(int i);
        virtual void writeFloat(float f);
        virtual void writeString(const String& s);
        virtual void writeVector3(const Vector3& v);
        virtual void writeQuaternion(const Quaternion& q);
        virtual void writePointer(void* p);
    };
}

#endif // __OGE_BINARYSERIALISER_H__
