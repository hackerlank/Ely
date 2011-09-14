/**
 * \fileDummySerializer.cpp
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#include "Utilities/Serializable.h"

namespace utils
{
    /** 
     * @note This class is only needed... to force the utilities lib 
     * to compile and export the Serialisable class!
     */
    class DummySerialisable : public Serialisable
    {
    public:
        virtual ~DummySerialisable() {}

        virtual void write(Serialiser* serialiser) {}
        virtual void read(Serialiser* serialiser) {}
        /**
         * This method is used by the ObjectFactory::fixupPass() to set
         * the pointers contained by the class to the new instances created
         * during a serialisation loading.
         */
        virtual bool fixup() { return true;}
    };
}
