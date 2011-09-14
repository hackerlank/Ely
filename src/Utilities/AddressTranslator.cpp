/**
 * \file AddressTranslator.cpp
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#include "oge/serialisation/OgeAddressTranslator.h"
#include "oge/logging/OgeLogManager.h"

namespace utils
{
    //-------------------------------------------------------------------------
    AddressTranslator::AddressTable AddressTranslator::sAddressTable;

    //-------------------------------------------------------------------------
    void AddressTranslator::addAddress(void* oldAddress, void* newAddress)
    {
        sAddressTable[oldAddress] = newAddress;
    }
    //-------------------------------------------------------------------------
    void* AddressTranslator::translateAddress(void* oldAddress)
    {
        AddressTable::iterator it = sAddressTable.find(oldAddress);
        if (it == sAddressTable.end())
        {
            LOGE(String("Potential Error: Couldn't find a translation for the address: ")
                + StringUtil::toString(oldAddress, 0, ' ', std::ios::hex | std::ios::showbase));
            return 0;
        }
        void* ptr = (void*)(*it).second;
        return ptr;
    }
    //-------------------------------------------------------------------------
    void AddressTranslator::resetAddresses()
    {
        sAddressTable.clear();
    }
    //-------------------------------------------------------------------------
}
