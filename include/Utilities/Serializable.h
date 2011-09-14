/**
 * \file Serializable.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef __OGE_SERIALISABLE_H__
#define __OGE_SERIALISABLE_H__

namespace utils
{
    /** 
     * All classes that must be serialisable must derive from this interface
     * to be serialisable.
     *
     * @note In the write method implementation the object class id MUST be
     *       the first data to be send.
     *
     * @author Steven 'lazalong' Gay
     */
    class Serialisable
    {
    public:
        virtual ~Serialisable() {}

        virtual void write(Serialiser* serialiser) = 0;
        virtual void read(Serialiser* serialiser) = 0;
        /**
         * This method is used by the ObjectFactory::fixupPass() to set
         * the pointers contained by the class to the new instances created
         * during a serialisation loading.
         */
        virtual bool fixup() = 0;
    };
}

#endif // __OGE_SERIALISABLE_H__
