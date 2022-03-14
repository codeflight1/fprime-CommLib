#include <SpaceWire/RMAPPacket.hpp>

#include <SpaceWire/SpaceWireAddr.hpp>
#include <SpaceWire/SpaceWireProtocolIDEnumAc.hpp>
#include <cstdlib>
#include <cstring> // memcpy

namespace SpaceWire {

U32 RMAPPacket::getLength() {
  U32 len = 8 + ((0b01000000 & this->m_Type.e) ? 4 : 0) // DataAddr
             + ((this->m_Type != RMAPPacketType::WriteReply) ? (
               4 + ((this->m_Type != RMAPPacketType::ReadCommand) ? this->m_Data.getSize() + 1 // Data Len + Header CRC + Data + Data CRC
               : 0)) : 0);
  len += addrPad(this->m_SourceAddr);
  len += m_DestAddr.getLength();
  return len;
}

RMAPEncodeStatus RMAPPacket::encode(Fw::Buffer& buffer) {
  U8 addrLen = 0;
  U32 len = this->getLength();
  U32 loc = 0;

  if (this->m_DestAddr.getType() != this->m_SourceAddr.getType()) {
    return RMAPEncodeStatus::ADDR_TYPE_MISMATCH;
  }
  if (this->m_DataLen > 16777216) { // 2**24
    return RMAPEncodeStatus::DATA_LEN_OVERRUN;
  }
  if (this->m_DataLen != this->m_Data.getSize() && this->m_Type != RMAPPacketType::WriteReply && this->m_Type != RMAPPacketType::ReadCommand) {
    return RMAPEncodeStatus::DATA_LEN_MISMATCH;
  }
  if (len != buffer.getSize()) {
    return RMAPEncodeStatus::BUFFER_LEN_MISMATCH;
  }

  // RMW Data + Mask length check

  if (this->m_SourceAddr.getType() == SpaceWireAddrType::PHYSICAL) {
    memcpy(buffer.getData(), this->m_DestAddr.getPhysicalAddr(), this->m_DestAddr.getLength());
    loc += this->m_DestAddr.getLength();
  }

  buffer.getData()[loc] = this->m_DestAddr.getLogicalAddr();
  loc++;

  buffer.getData()[loc] = (U8) SpaceWireProtocolID::RMAP;
  loc++;

  U8 flags = (U8) this->m_Type.e | (this->m_Verify ? 0b00010000 : 0) | (this->m_Ack ? 0b00001000 : 0) | (this->m_Increment ? 0b00000100 : 0) | (0b00000011 & (addrLen/4));
  buffer.getData()[loc] = flags;
  loc++;

  if (0b01000000 & this->m_Type.e) {
    buffer.getData()[loc] = this->m_DestKey;
  }else {
    buffer.getData()[loc] = (U8) this->m_Status.e;
  }
  loc++;

  if (this->m_SourceAddr.getType() == SpaceWireAddrType::PHYSICAL && 0b01000000 & this->m_Type.e) {
    memcpy(buffer.getData() + loc, this->m_SourceAddr.getPhysicalAddr(), this->m_SourceAddr.getLength());
    loc += this->m_SourceAddr.getLength();
  }

  buffer.getData()[loc] = this->m_SourceAddr.getLogicalAddr();
  loc++;

  buffer.getData()[loc + 0] = static_cast<U8>(this->m_TransID >> 8);
  buffer.getData()[loc + 1] = static_cast<U8>(this->m_TransID);
  loc += 2;

  if (0b01000000 & this->m_Type.e) {
    buffer.getData()[loc] = this->m_ExtDataAddr;
    buffer.getData()[loc + 1] = static_cast<U8>(this->m_DataAddr >> 24);
    buffer.getData()[loc + 2] = static_cast<U8>(this->m_DataAddr >> 16);
    buffer.getData()[loc + 3] = static_cast<U8>(this->m_DataAddr >> 8);
    buffer.getData()[loc + 4] = static_cast<U8>(this->m_DataAddr);
    loc += 5;
  }else if (this->m_Type == RMAPPacketType::WriteReply) {
    U8 replyCRC = RMAPPacket::CRC(buffer.getData() + this->m_DestAddr.getLength(), loc - this->m_DestAddr.getLength());
    buffer.getData()[loc] = replyCRC;
    return RMAPEncodeStatus::SUCCESS;
  }else {
    buffer.getData()[loc] = 0;
    loc++;
  }

  buffer.getData()[loc + 0] = static_cast<U8>(this->m_DataLen >> 16);
  buffer.getData()[loc + 1] = static_cast<U8>(this->m_DataLen >> 8);
  buffer.getData()[loc + 2] = static_cast<U8>(this->m_DataLen);
  loc += 3;

  U8 headerCRC = RMAPPacket::CRC(buffer.getData() + this->m_DestAddr.getLength(), loc - this->m_DestAddr.getLength());
  buffer.getData()[loc] = headerCRC;
  loc++;

  if (this->m_Type == RMAPPacketType::ReadCommand) {
    return RMAPEncodeStatus::SUCCESS;
  }

  memcpy(buffer.getData() + loc, this->m_Data.getData(), this->m_Data.getSize());
  loc += this->m_Data.getSize();

  U8 dataCRC = RMAPPacket::CRC(this->m_Data.getData(), this->m_Data.getSize());
  buffer.getData()[loc] = dataCRC;

  return RMAPEncodeStatus::SUCCESS;
}

RMAPDecodeStatus RMAPPacket::decode(RMAPPacket& packet, const Fw::Buffer& buffer) {
  packet = RMAPPacket();

  U32 loc = 0;
  while (buffer.getData()[loc] < 32) { // skip over path address
    loc++;
  }
  U8 bgloc = loc;
  U8* destpath = (U8*) malloc(loc);
  memcpy(destpath, buffer.getData(), loc);
  packet.setDestAddr(SpaceWireAddr(buffer.getData()[loc], destpath, loc, loc > 0 ? SpaceWireAddrType::PHYSICAL : SpaceWireAddrType::LOGICAL));
  free(destpath);
  loc++;

  if (static_cast<SpaceWireProtocolID::t>(buffer.getData()[loc]) != SpaceWireProtocolID::RMAP) {
    return RMAPDecodeStatus::WRONG_PROTOCOL;
  }
  loc++;

  U8 flags = buffer.getData()[loc];
  loc++;

  // There's probably a better way of doing this
  if (0b10000000 & flags) {
    return RMAPDecodeStatus::UNK_PACKET_TYPE;
  }else if ((0b11111100 & flags) == RMAPPacketType::RMWCommand) {
    packet.setType(RMAPPacketType::RMWCommand);
  }else if ((0b11111100 & flags) == RMAPPacketType::RMWReply) {
    packet.setType(RMAPPacketType::RMWReply);
  }else if ((0b11111000 & flags) == RMAPPacketType::ReadCommand) {
    packet.setType(RMAPPacketType::ReadCommand);
  }else if ((0b11111000 & flags) == RMAPPacketType::ReadReply) {
    packet.setType(RMAPPacketType::ReadReply);
  }else if ((0b00101000 & flags) == RMAPPacketType::WriteReply) {
    packet.setType(RMAPPacketType::WriteReply);
  }else if ((0b01100000 & flags) == RMAPPacketType::WriteCommand) {
    packet.setType(RMAPPacketType::WriteCommand);
  }

  packet.setVerify   (0b00010000 & flags);
  packet.setAck      (0b00001000 & flags);
  packet.setIncrement(0b00000100 & flags);

  U8 srcpathlen = ((U8) 0b00000011 & flags) * 4;

  if (0b01000000 & flags) {
    packet.setDestKey(buffer.getData()[loc]);
  }else {
    packet.setStatus(static_cast<RMAPStatus::t>(buffer.getData()[loc]));
  }
  loc++;

  U8* srcpath = (U8*) malloc(srcpathlen);
  if (0b01000000 & packet.m_Type.e) {
    memcpy(srcpath, buffer.getData() + loc, srcpathlen);
  }
  packet.setSourceAddr(SpaceWireAddr(buffer.getData()[loc + srcpathlen], destpath, srcpathlen, srcpathlen > 0 ? SpaceWireAddrType::PHYSICAL : SpaceWireAddrType::LOGICAL));
  free(srcpath);
  loc += srcpathlen + 1;

  packet.setTransID(static_cast<U16>(
      ((buffer.getData()[loc + 1]) << 0) |
      ((buffer.getData()[loc + 0]) << 8)
  ));
  loc += 2;

  // ExtDataAddr/0/replyCRC
  if (0b01000000 & flags) {
    packet.setExtDataAddr(buffer.getData()[loc]);
    packet.setDataAddr(
        (static_cast<U32>(buffer.getData()[loc + 4]) << 0)  |
        (static_cast<U32>(buffer.getData()[loc + 3]) << 8)  |
        (static_cast<U32>(buffer.getData()[loc + 2]) << 16) |
        (static_cast<U32>(buffer.getData()[loc + 1]) << 24)
    );
    loc += 5;
  }else if (packet.getType() == RMAPPacketType::WriteReply) {
    U8 newCRC = RMAPPacket::CRC(buffer.getData(), loc);
    if (newCRC == buffer.getData()[loc]) {
      return RMAPDecodeStatus::SUCCESS;
    }else {
      return RMAPDecodeStatus::BAD_HEADER_CRC;
    }
  }else {
    loc++; // skip over reserved byte
  }

  packet.setDataLen(
      (static_cast<U32>(buffer.getData()[loc + 2]) << 0)  |
      (static_cast<U32>(buffer.getData()[loc + 1]) << 8)  |
      (static_cast<U32>(buffer.getData()[loc + 0]) << 16)
  );
  loc += 3;

  U8 newCRC = RMAPPacket::CRC(buffer.getData() + bgloc, loc-bgloc);
  if (newCRC != buffer.getData()[loc]) {
    return RMAPDecodeStatus::BAD_HEADER_CRC;
  }
  loc++;

  if (packet.getType() == RMAPPacketType::ReadCommand) {
    return RMAPDecodeStatus::SUCCESS;
  }

  if (packet.getVerify()) {
    U8 dataCRC = RMAPPacket::CRC(buffer.getData() + loc, packet.getDataLen());
    if (dataCRC != buffer.getData()[loc + packet.getDataLen()]) {
      return RMAPDecodeStatus::BAD_DATA_CRC;
    }
  }

  Fw::Buffer databuf = Fw::Buffer((U8*) malloc(packet.getDataLen()), packet.getDataLen(), 0x123);

  memcpy(databuf.getData(), buffer.getData() + loc, packet.getDataLen());

  packet.setData(databuf);

  return RMAPDecodeStatus::SUCCESS;
}

U8 RMAPPacket::addrPad(SpaceWireAddr& addr) {
  U8 len = addr.getLength();
  while (len % 4 != 0) {
    len++;
  }

  U8* addrData = (U8*) malloc(len);

  for (NATIVE_INT_TYPE i = 0; i < len-addr.getLength(); i++) {
    addrData[i] = 0;
  }

  for (NATIVE_INT_TYPE i = 0; i < addr.getLength(); i++) {
    addrData[i+(len-addr.getLength())] = addr.getPhysicalAddr()[i];
  }

  addr.setPhysicalAddr(addrData, len);

  free(addrData);

  return len;
}

bool RMAPPacket::operator==(const RMAPPacket& src) const {
  if (0b01000000 & src.m_Type.e) {
    SpaceWireAddr a = src.m_SourceAddr;
    SpaceWireAddr b = this->m_SourceAddr;
    (void) RMAPPacket::addrPad(a);
    (void) RMAPPacket::addrPad(b);
    if (!(a == b)) {
      return false;
    }
  }
  if ((src.m_DestAddr == this->m_DestAddr) &&
      (src.m_Type == this->m_Type) &&
      (src.getVerify() == this->getVerify()) &&
      (src.getAck() == this->getAck()) &&
      (src.getIncrement() == this->getIncrement()) &&
      (src.m_Status == this->m_Status) &&
      (src.m_DestKey == this->m_DestKey) &&
      (src.m_TransID == this->m_TransID) &&
      (src.m_ExtDataAddr == this->m_ExtDataAddr) &&
      (src.m_DataAddr == this->m_DataAddr) &&
      (src.m_DataLen == this->m_DataLen) &&
      (src.m_Data.getSize() == this->m_Data.getSize())) {
    for (NATIVE_INT_TYPE i = 0; i < src.m_Data.getSize(); i++) {
      if (src.m_Data.getData()[i] != this->m_Data.getData()[i]) {
        return false;
      }
    }
    return true;
  }else {
    return false;
  }
}

bool RMAPPacket::getVerify() const {
    return this->m_Verify ? true : (0b00010000 & (U8) this->m_Type.e);
}

bool RMAPPacket::getAck() const {
    return this->m_Ack ? true : (0b00001000 & (U8) this->m_Type.e);
}

bool RMAPPacket::getIncrement() const {
    return this->m_Increment ? true : (0b00000100 & (U8) this->m_Type.e);
}

#if FW_SERIALIZABLE_TO_STRING  || BUILD_UT

void RMAPPacket::toString(Fw::StringBase& text) const {

    static const char * formatString =
       "("
       "Type = %s, "
       "Verify = %s, "
       "Ack = %s, "
       "Increment = %s, "
       "Status = %s, "
       "DestKey = %u, "
       "TransID = %u, "
       "ExtDataAddr = %u, "
       "DataAddr = %u, "
       "DataLen = %u, "
       "Data = %s"
       ")";

    // declare strings to hold any serializable toString() arguments

    Fw::String TypeStr;
    this->m_Type.toString(TypeStr);

    Fw::String StatusStr;
    this->m_Status.toString(StatusStr);

    Fw::String DataStr;
    this->m_Data.toString(DataStr);

    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    (void)snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,formatString
       ,TypeStr.toChar()
       ,this->m_Verify?"T":"F"
       ,this->m_Ack?"T":"F"
       ,this->m_Increment?"T":"F"
       ,StatusStr.toChar()
       ,this->m_DestKey
       ,this->m_TransID
       ,this->m_ExtDataAddr
       ,this->m_DataAddr
       ,this->m_DataLen
       ,DataStr.toChar()
    );
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

    text = outputString;
}

#endif

/* The table used to calculate the CRC for a buffer */
U8 RMAP_CRCTable[] = {
  0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75,
  0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b,
  0x1c, 0x8d, 0xff, 0x6e, 0x1b, 0x8a, 0xf8, 0x69,
  0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67,
  0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc, 0x4d,
  0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43,
  0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0, 0x51,
  0x2a, 0xbb, 0xc9, 0x58, 0x2d, 0xbc, 0xce, 0x5f,
  0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94, 0x05,
  0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b,
  0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88, 0x19,
  0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17,
  0x48, 0xd9, 0xab, 0x3a, 0x4f, 0xde, 0xac, 0x3d,
  0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33,
  0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0, 0x21,
  0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f,
  0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04, 0x95,
  0xee, 0x7f, 0x0d, 0x9c, 0xe9, 0x78, 0x0a, 0x9b,
  0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18, 0x89,
  0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87,
  0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c, 0xad,
  0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3,
  0xc4, 0x55, 0x27, 0xb6, 0xc3, 0x52, 0x20, 0xb1,
  0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf,
  0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74, 0xe5,
  0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb,
  0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68, 0xf9,
  0x82, 0x13, 0x61, 0xf0, 0x85, 0x14, 0x66, 0xf7,
  0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c, 0xdd,
  0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3,
  0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50, 0xc1,
  0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf,
};

/* Calculate an 8-bit CRC for the given buffer */
U8 RMAPPacket::CRC(U8* buf, U32 len) {
  U32 i;
  U8 crc;
  /* CRC = 0 */
  crc = 0;
  /* for each byte in the buffer */
  for (i = 0; i < len; i++)
  {
    /* The value of the byte from the buffer is */
    /* XORed with the current CRC value. */
    /* The result is then used to lookup the */
    /* new CRC value from the CRC lookup table */
    crc = RMAP_CRCTable[crc ^ *buf++];
  }
  /* return the CRC */
  return crc;
}

} // end namespace SpaceWire
