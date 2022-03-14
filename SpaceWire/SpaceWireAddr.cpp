#include <SpaceWire/SpaceWireAddrSerializableAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringUtils.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/String.hpp>
#endif
#include <cstring>
namespace SpaceWire {
// public methods

SpaceWireAddr::SpaceWireAddr(): Serializable() {
    this->m_LogicalAddr = 0;
    for (NATIVE_INT_TYPE _mem = 0; _mem < 8; _mem++) {
        this->m_PhysicalAddr[_mem] = 0;
    }
    this->m_Type = SpaceWire::SpaceWireAddrType::PHYSICAL;
    this->m_Length = 0;
}

SpaceWireAddr::SpaceWireAddr(const SpaceWireAddr& src) : Serializable() {
    this->set(src.m_LogicalAddr, src.m_PhysicalAddr, src.m_Length, src.m_Type);
}

SpaceWireAddr::SpaceWireAddr(const SpaceWireAddr* src) : Serializable() {
    FW_ASSERT(src);
    this->set(src->m_LogicalAddr, src->m_PhysicalAddr, src->m_Length, src->m_Type);
}

SpaceWireAddr::SpaceWireAddr(U8 LogicalAddr, const U8* PhysicalAddr, U8 Length, const SpaceWire::SpaceWireAddrType& Type) : Serializable() {
    this->set(LogicalAddr, PhysicalAddr, Length, Type);
}

SpaceWireAddr& SpaceWireAddr::operator=(const SpaceWireAddr& src) {
    if(this == &src) {
            return *this;
    }

    this->set(src.m_LogicalAddr, src.m_PhysicalAddr, src.m_Length, src.m_Type);
    return *this;
}

bool SpaceWireAddr::operator==(const SpaceWireAddr& src) const {
    return (
        (src.m_LogicalAddr == this->m_LogicalAddr) &&
        (src.m_PhysicalAddr == this->m_PhysicalAddr) &&
        (src.m_Type == this->m_Type) &&
        (src.m_Length == this->m_Length) &&
        true);
}

void SpaceWireAddr::set(U8 LogicalAddr, const U8* PhysicalAddr, U8 Length, const SpaceWire::SpaceWireAddrType& Type) {
    this->m_LogicalAddr = LogicalAddr;
    FW_ASSERT(Length <= 8);
    for (NATIVE_INT_TYPE _mem = 0; _mem < FW_MIN(Length,8); _mem++) {
        this->m_PhysicalAddr[_mem] = PhysicalAddr[_mem];
    }
    this->m_Type = Type;
    this->m_Length = Length;
}

U8 SpaceWireAddr::getLogicalAddr() const {
    return this->m_LogicalAddr;
}

const U8* SpaceWireAddr::getPhysicalAddr() const {
    return this->m_PhysicalAddr;
}

const SpaceWire::SpaceWireAddrType& SpaceWireAddr::getType() const {
    return this->m_Type;
}

U8 SpaceWireAddr::getLength() const {
    return this->m_Length;
}

void SpaceWireAddr::setLogicalAddr(U8 val) {
    this->m_LogicalAddr = val;
}
void SpaceWireAddr::setPhysicalAddr(const U8* val, U8 length) {
    this->m_Length = length;
    for (NATIVE_INT_TYPE _mem = 0; _mem < FW_MIN(length,8); _mem++) {
        this->m_PhysicalAddr[_mem] = val[_mem];
    }
}
void SpaceWireAddr::setType(const SpaceWire::SpaceWireAddrType& val) {
    this->m_Type = val;
}

Fw::SerializeStatus SpaceWireAddr::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    // serialize type ID
    stat = buffer.serialize(static_cast<U32>(SpaceWireAddr::TYPE_ID));
#endif

    stat = buffer.serialize(this->m_LogicalAddr);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    for (NATIVE_INT_TYPE _mem = 0; _mem < 8; _mem++) {
        stat = buffer.serialize(this->m_PhysicalAddr[_mem]);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
    }
    stat = buffer.serialize(this->m_Type);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.serialize(this->m_Length);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

Fw::SerializeStatus SpaceWireAddr::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::SerializeStatus stat;

#if FW_SERIALIZATION_TYPE_ID
    U32 typeId;

    stat = buffer.deserialize(typeId);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    if (typeId != SpaceWireAddr::TYPE_ID) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
#endif

    stat = buffer.deserialize(this->m_LogicalAddr);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    for (NATIVE_INT_TYPE _mem = 0; _mem < 8; _mem++) {
        stat = buffer.deserialize(this->m_PhysicalAddr[_mem]);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
    }
    stat = buffer.deserialize(this->m_Type);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    stat = buffer.deserialize(this->m_Length);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    return stat;
}

#if FW_SERIALIZABLE_TO_STRING  || BUILD_UT

void SpaceWireAddr::toString(Fw::StringBase& text) const {

    static const char * formatString =
       "("
       "LogicalAddr = %u, "
       "PhysicalAddr = "
       "[%u]"
       "[%u]"
       "[%u]"
       "[%u]"
       "[%u]"
       "[%u]"
       "[%u]"
       "[%u] "
       "Type = %s, "
       "Length = %u"
       ")";

    // declare strings to hold any serializable toString() arguments


    Fw::String TypeStr;
    this->m_Type.toString(TypeStr);

    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    (void)snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,formatString
       ,this->m_LogicalAddr
       ,this->m_PhysicalAddr[0]
       ,this->m_PhysicalAddr[1]
       ,this->m_PhysicalAddr[2]
       ,this->m_PhysicalAddr[3]
       ,this->m_PhysicalAddr[4]
       ,this->m_PhysicalAddr[5]
       ,this->m_PhysicalAddr[6]
       ,this->m_PhysicalAddr[7]
       ,TypeStr.toChar()
       ,this->m_Length
    );
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

    text = outputString;
}

#endif

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const SpaceWireAddr& obj) {
        Fw::String str;
        obj.toString(str);
        os << str.toChar();
        return os;
    }
#endif
} // end namespace SpaceWire
