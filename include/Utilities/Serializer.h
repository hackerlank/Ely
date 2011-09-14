/**
 * \file Serializer.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef __OGE_SERIALISER_H__
#define __OGE_SERIALISER_H__

#include "oge/OgeUtilitiesPrerequisites.h"
#include "oge/OgeString.h"

namespace utils
{
    /** 
     * The interface Serialiser must be implemented by all classes that want
     * to be able to serialise instances (whose classes must implement 
     * the Serialisable interface).
     *
     * @author Steven 'lazalong' Gay
     */
    class Serialiser
    {
    protected:
        bool mReadMode;
        bool mWriteMode;
        DataStream* mDataStream;

    public:
        Serialiser(DataStream* dataStream) :
            mReadMode(false), mWriteMode(false), mDataStream(dataStream) {};
        virtual ~Serialiser() {};

        inline bool getReadMode() const { return mReadMode; };
        inline bool getWriteMode() const { return mWriteMode; };

        // Read methods (must have a write equivalent)
        virtual int readInt() = 0;
        virtual float readFloat() = 0;
        virtual String readString() = 0;
        virtual Vector3 readVector3() = 0;
        virtual Quaternion readQuaternion() = 0;
        virtual void* readPointer() = 0;

         // Write methods (must have a read equivalent)
        virtual void writeInt        (int i) = 0;
        virtual void writeFloat      (float f) = 0;
        virtual void writeString     (const String & s) = 0;
        virtual void writeVector3    (const Vector3& v) = 0;
        virtual void writeQuaternion (const Quaternion& q) = 0;
        virtual void writePointer    (void* p) = 0;
    };
}

#endif // __OGE_SERIALISER_H__
