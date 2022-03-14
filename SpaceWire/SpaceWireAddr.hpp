/*
 * SpaceWireAddr.hpp
 *
 *  Author:     codeflight
 *
 */
#ifndef SPACEWIREADDR_HPP_
#define SPACEWIREADDR_HPP_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/StringType.hpp>
#include <cstdio> // snprintf
#ifdef BUILD_UT
#include <iostream>
#include <Fw/Types/String.hpp>
#endif
#endif
#include <SpaceWire/SpaceWireAddrTypeEnumAc.hpp>

namespace SpaceWire {

class SpaceWireAddr : public Fw::Serializable {


public:

    enum {
        SERIALIZED_SIZE =
        sizeof(U8) +
        sizeof(U8)*8 +
        SpaceWire::SpaceWireAddrType::SERIALIZED_SIZE +
        sizeof(U8)
    }; //!< serializable size of SpaceWireAddr

    SpaceWireAddr(); //!< Default constructor
    SpaceWireAddr(const SpaceWireAddr* src); //!< pointer copy constructor
    SpaceWireAddr(const SpaceWireAddr& src); //!< reference copy constructor
    SpaceWireAddr(U8 LogicalAddr, const U8* PhysicalAddr, U8 Length, const SpaceWire::SpaceWireAddrType& Type); //!< constructor with arguments
    SpaceWireAddr& operator=(const SpaceWireAddr& src); //!< equal operator
    bool operator==(const SpaceWireAddr& src) const; //!< equality operator
#ifdef BUILD_UT
    // to support GoogleTest framework in unit tests
    friend std::ostream& operator<<(std::ostream& os, const SpaceWireAddr& obj);
#endif

    void set(U8 LogicalAddr, const U8* PhysicalAddr, U8 Length, const SpaceWire::SpaceWireAddrType& Type); //!< set values

    U8 getLogicalAddr() const; //!< get member LogicalAddr
    const U8* getPhysicalAddr() const; //!< get member PhysicalAddr
    const SpaceWire::SpaceWireAddrType& getType() const; //!< get member Type
    U8 getLength() const; //!< get member Length

    void setLogicalAddr(U8 val); //!< set member LogicalAddr
    void setPhysicalAddr(const U8* val, U8 length); //!< set member PhysicalAddr
    void setType(const SpaceWire::SpaceWireAddrType& val); //!< set member Type

    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const; //!< serialization function
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer); //!< deserialization function
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(Fw::StringBase& text) const; //!< generate text from serializable
#endif
protected:

    enum {
        TYPE_ID = 0x77674CE9 //!< type id
    };

    U8 m_LogicalAddr; //<! LogicalAddr -
    U8 m_PhysicalAddr[8]; //<! PhysicalAddr -

    SpaceWire::SpaceWireAddrType m_Type; //<! Type -
    U8 m_Length; //<! Length -
private:

};
} // end namespace SpaceWire
#endif /* SPACEWIREADDR_HPP_ */
