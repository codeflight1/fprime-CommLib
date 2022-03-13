#include <SpaceWire/RMAPPacket.hpp>

#include <SpaceWire/SpaceWireAddrSerializableAc.hpp>
#include <SpaceWire/SpaceWireProtocolIDEnumAc.hpp>
#include <cstdlib>
#include <cstring> // memcpy

namespace SpaceWire {
U8* RMAPPacket::encode() {
  U8 addrLen = 0;
  U32 len = 8 + ((0b01000000 & this->m_Type.e) ? 4 : 0) // DataAddr
             + ((this->m_Type != RMAPPacketType::WriteReply) ? (
               4 + ((this->m_Type != RMAPPacketType::ReadCommand) ? this->m_Data.getSize() + 1 // Data Len + Header CRC + Data + Data CRC
               : 0)) : 0);
  U32 loc = 0;

  if (this->m_DestAddr.getType() != this->m_SourceAddr.getType()) {
    // TYPE_MISMATCH
  }
  if (this->m_DataLen > 16777216) { // 2**24
    // DATA LEN TOO LONG
  }
  if (this->m_DataLen != this->m_Data.getSize() && this->m_Type != RMAPPacketType::WriteReply && this->m_Type != RMAPPacketType::ReadCommand) {
    // LENGTH MISMATCH
  }

  if (this->m_SourceAddr.getType() == SpaceWireAddrType::PHYSICAL) {
    addrLen = addrPad(this->m_SourceAddr);
    len += addrLen;
    len += m_DestAddr.getLength();
  }

  U8* buff = (U8*) malloc(len);

  if (this->m_SourceAddr.getType() == SpaceWireAddrType::PHYSICAL) {
    NATIVE_INT_TYPE l;
    memcpy(buff, this->m_DestAddr.getPhysicalAddr(l), this->m_DestAddr.getLength());
    loc += this->m_DestAddr.getLength();
  }

  buff[loc] = this->m_DestAddr.getLogicalAddr();
  loc++;

  buff[loc] = (U8) SpaceWireProtocolID::RMAP;
  loc++;

  U8 flags = (U8) this->m_Type.e | (this->m_Verify ? 0b00010000 : 0) | (this->m_Ack ? 0b00001000 : 0) | (this->m_Increment ? 0b00000100 : 0) | (0b00000011 & (addrLen/4));
  buff[loc] = flags;
  loc++;

  if (0b01000000 & this->m_Type.e) {
    buff[loc] = this->m_DestKey;
  }else {
    buff[loc] = (U8) this->m_Status.e;
  }
  loc++;

  if (this->m_SourceAddr.getType() == SpaceWireAddrType::PHYSICAL) {
    NATIVE_INT_TYPE l;
    memcpy(buff + loc, this->m_SourceAddr.getPhysicalAddr(l), this->m_SourceAddr.getLength());
    loc += this->m_SourceAddr.getLength();
  }

  buff[loc] = this->m_SourceAddr.getLogicalAddr();
  loc++;

  buff[loc + 0] = static_cast<U8>(this->m_TransID >> 8);
  buff[loc + 1] = static_cast<U8>(this->m_TransID);
  loc += 2;

  if (0b01000000 & this->m_Type.e) {
    buff[loc] = this->m_ExtDataAddr;
    buff[loc + 1] = static_cast<U8>(this->m_DataAddr >> 24);
    buff[loc + 2] = static_cast<U8>(this->m_DataAddr >> 16);
    buff[loc + 3] = static_cast<U8>(this->m_DataAddr >> 8);
    buff[loc + 4] = static_cast<U8>(this->m_DataAddr);
    loc += 5;
  }else if (this->m_Type == RMAPPacketType::WriteReply) {
    U8 replyCRC = RMAPPacket::CRC(buff + this->m_DestAddr.getLength(), loc - this->m_DestAddr.getLength());
    buff[loc] = replyCRC;
    return buff;
  }else {
    buff[loc] = 0;
    loc++;
  }

  buff[loc + 0] = static_cast<U8>(this->m_DataLen >> 16);
  buff[loc + 1] = static_cast<U8>(this->m_DataLen >> 8);
  buff[loc + 2] = static_cast<U8>(this->m_DataLen);
  loc += 3;

  U8 headerCRC = RMAPPacket::CRC(buff + this->m_DestAddr.getLength(), loc - this->m_DestAddr.getLength());
  buff[loc] = headerCRC;
  loc++;

  if (this->m_Type == RMAPPacketType::ReadCommand) {
    return buff;
  }

  memcpy(buff + loc, this->m_Data.getData(), this->m_Data.getSize());
  loc += this->m_Data.getSize();

  U8 dataCRC = RMAPPacket::CRC(this->m_Data.getData(), this->m_Data.getSize());
  buff[loc] = dataCRC;

  return buff;
}

U8 RMAPPacket::addrPad(SpaceWireAddr& addr) const {
  NATIVE_INT_TYPE l;
  (void) addr.getPhysicalAddr(l);
  U8* addrData = (U8*) malloc(l);

  U8 len = addr.getLength();
  while (len % 4 != 0) {
    len++;
  }

  for (NATIVE_INT_TYPE i = 0; i < len-addr.getLength(); i++) {
    addrData[i] = 0;
  }

  for (NATIVE_INT_TYPE i = 0; i < addr.getLength(); i++) {
    addrData[i+(len-addr.getLength())] = addr.getPhysicalAddr(l)[i];
  }

  addr.setPhysicalAddr(addrData, l);
  addr.setLength(len);

  free(addrData);

  return len;
}

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
