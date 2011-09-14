/**
 * \file AddressTranslator.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef ADDRESSTRANSLATOR_H__
#define ADDRESSTRANSLATOR_H__

namespace utils
{
    /**
     * This class saves in memory the addresses of newly created objects
     * to be able to translate the "old" pointers to the new ones.
     *
     * @author Steven 'lazalong' Gay
     */
    class AddressTranslator
    {
    private:
        typedef OgeHashMap<void*, void*> AddressTable;
        static AddressTable sAddressTable;

    public:
        /**
         * Save the memory address of a newly created object against its old one.
         */
        static void addAddress(void* oldAddress, void* newAddress);
        /** 
         * Retrieve the address of a new instantiation corresponding
         * to an old instantiation (using its old address).
         */
        static void* translateAddress(void* oldAddress);
        static void resetAddresses();
    };
}

#endif // __OGE_ADDRESSTRANSLATOR_H__
