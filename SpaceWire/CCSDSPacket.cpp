#include <SpaceWire/CCSDSPacket.hpp>

#include <SpaceWire/SpaceWireProtocolIDEnumAc.hpp>

#include <cstring> // memcpy
#include <cstdlib> // malloc

namespace SpaceWire {
  CCSDSEncodeStatus CCSDSPacket::encode(Fw::Buffer& buffer) {
    if (buffer.getSize() != this->getLength()) {
      return CCSDSEncodeStatus::SIZE_MISMATCH;
    }

    if (this->m_APID >= 2048) {
      return CCSDSEncodeStatus::APID_OVERFLOW;
    }

    if (this->m_SeqCnt >= 16384) {
      return CCSDSEncodeStatus::SEQCNT_OVERFLOW;
    }

    if (this->m_Data.getSize() > 65536) {
      return CCSDSEncodeStatus::DATA_OVERFLOW;
    }

    U8 loc = 0;

    if (this->m_TargetAddr.getType() == SpaceWireAddrType::PHYSICAL) {
      memcpy(buffer.getData(), this->m_TargetAddr.getPhysicalAddr(), this->m_TargetAddr.getLength());
      loc += this->m_TargetAddr.getLength();
    }

    buffer.getData()[loc + 0] = this->m_TargetAddr.getLogicalAddr();
    buffer.getData()[loc + 1] = SpaceWireProtocolID::CCSDS;
    buffer.getData()[loc + 2] = 0x00;
    buffer.getData()[loc + 3] = this->m_UserApplication;
    loc += 4;

    U16 id = (0b0000000000000000) |
             (0b0001000000000000 & this->m_Type) |
             (0b0000100000000000 & this->m_SecHdrFlag) |
             (0b0000011111111111 & this->m_APID);

    U16 SeqCtrl = (0b1100000000000000 & this->m_SeqFlags.e << 14) |
                  (0b0011111111111111 & this->m_SeqCnt);

    buffer.getData()[loc + 0] = static_cast<U8>(id >> 8);
    buffer.getData()[loc + 1] = static_cast<U8>(id);
    buffer.getData()[loc + 2] = static_cast<U8>(SeqCtrl >> 8);
    buffer.getData()[loc + 3] = static_cast<U8>(SeqCtrl);
    buffer.getData()[loc + 4] = static_cast<U8>((this->m_Data.getSize() - 1) >> 8);
    buffer.getData()[loc + 5] = static_cast<U8>((this->m_Data.getSize() - 1));

    memcpy(buffer.getData() + loc + 6, this->m_Data.getData(), this->m_Data.getSize());

    return CCSDSEncodeStatus::SUCCESS;
  }

  CCSDSDecodeStatus CCSDSPacket::decode(CCSDSPacket& packet, Fw::Buffer& buffer) {
    U32 loc = 0;
    while (buffer.getData()[loc] < 32) { // skip over path address
      loc++;
    }

    packet.setTargetAddr(SpaceWireAddr(buffer.getData()[loc], nullptr, 0, SpaceWireAddrType::LOGICAL));
    loc++;

    if (static_cast<SpaceWireProtocolID::t>(buffer.getData()[loc]) != SpaceWireProtocolID::CCSDS) {
      return CCSDSDecodeStatus::WRONG_PROTOCOL;
    }
    loc++;

    if (buffer.getData()[loc] != 0x00) {
      return CCSDSDecodeStatus::RSRVD_NZ;
    }
    loc++;

    packet.setUserApplication(buffer.getData()[loc]);
    loc++;

    U16 id = static_cast<U16>(
        ((buffer.getData()[loc + 1]) << 0) |
        ((buffer.getData()[loc + 0]) << 8)
    );
    loc += 2;

    packet.setType      (0b0001000000000000 & id);
    packet.setSecHdrFlag(0b0000100000000000 & id);
    packet.setAPID      (0b0000011111111111 & id);

    U16 SeqCtrl = static_cast<U16>(
        ((buffer.getData()[loc + 1]) << 0) |
        ((buffer.getData()[loc + 0]) << 8)
    );
    loc += 2;

    packet.setSeqFlags(static_cast<CCSDSSeqType::t>((0b1100000000000000 & SeqCtrl) >> 14));
    packet.setSeqCnt(0b0011111111111111 & SeqCtrl);

    U16 datalen = static_cast<U16>(
        ((buffer.getData()[loc + 1]) << 0) |
        ((buffer.getData()[loc + 0]) << 8)
    ) + 1;
    loc += 2;

    Fw::Buffer databuf = Fw::Buffer((U8*) malloc(datalen), datalen, 0x01);
    memcpy(databuf.getData(), buffer.getData() + loc, datalen);
    packet.setData(databuf);

    return CCSDSDecodeStatus::SUCCESS;
  }

  bool CCSDSPacket::operator==(const CCSDSPacket& src) const {
    if (!(src.m_TargetAddr == this->m_TargetAddr) &&
         (src.m_UserApplication == this->m_UserApplication) &&
         (src.m_Type == this->m_Type) &&
         (src.m_SecHdrFlag == this->m_SecHdrFlag) &&
         (src.m_APID == this->m_APID) &&
         (src.m_SeqFlags == this->m_SeqFlags) &&
         (src.m_SeqCnt == this->m_SeqCnt)) {
      return false;
    }
    for (NATIVE_INT_TYPE i = 0; i < src.m_Data.getSize(); i++) {
      if (src.m_Data.getData()[i] != this->m_Data.getData()[i]) {
        return false;
      }
    }
    return true;
  }
} /* SpaceWire */
