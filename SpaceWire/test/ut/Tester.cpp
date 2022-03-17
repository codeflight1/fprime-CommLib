// ======================================================================
// \title  Tester.hpp
// \author codeflight
// \brief  cpp file for SpaceWireController test harness implementation class
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace SpaceWire {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
#if FW_OBJECT_NAMES == 1
      SpaceWireControllerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("SpaceWireDecoder")
#else
      SpaceWireControllerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    RMAPDispatchTest()
  {
    U8 dat[1] = {0x01};

    RMAPPacket packet = RMAPPacket();
    packet.set(SpaceWireAddr(0x54, dat, 0, SpaceWireAddrType::LOGICAL),
               SpaceWireAddr(0x76, dat, 0, SpaceWireAddrType::LOGICAL),
               RMAPPacketType::WriteCommand, false, false, true, RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x1000, (U32) 0, Fw::Buffer(nullptr, 0, 0));

    Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

    (void) packet.encode(outbuf);

    this->clearHistory();

    this->invoke_to_registerTransID(1, 0x05, true);
    this->component.doDispatch();

    this->invoke_to_dataIn(0, outbuf);
    this->component.doDispatch();

    ASSERT_from_RMAPout_SIZE(1);
    ASSERT_from_RMAPout(0, packet);
  }

  void Tester ::
    CCSDSDispatchTest()
  {
    U8 dat[1] = {0x01};

    CCSDSPacket packet = CCSDSPacket();
    packet.set(SpaceWireAddr(0x54, dat, 0, SpaceWireAddrType::LOGICAL),
               0x01, false, false, 0x57, CCSDSSeqType::UnSegmented, 0x00, Fw::Buffer(nullptr, 0, 0));

    Fw::Buffer outbuf = Fw::Buffer((U8*) malloc(packet.getLength()), packet.getLength(), 0x02);

    (void) packet.encode(outbuf);

    this->clearHistory();

    this->invoke_to_registerAPID(0, 0x57, true);
    this->component.doDispatch();

    this->invoke_to_dataIn(0, outbuf);
    this->component.doDispatch();

    ASSERT_from_CCSDSout_SIZE(1);
    ASSERT_from_CCSDSout(0, packet);
  }

  void Tester ::
    rawDispatchTest()
  {
    U8 d[3] = {0x22, 0x03, 0x05};
    Fw::Buffer outbuf = Fw::Buffer(d, 3, 0x02);

    this->clearHistory();

    this->invoke_to_registerProtocol(0, 0x03, true);
    this->component.doDispatch();

    this->invoke_to_dataIn(0, outbuf);
    this->component.doDispatch();

    ASSERT_from_rawOut_SIZE(1);
    Fw::Buffer b = this->fromPortHistory_rawOut->at(0).fwBuffer;

    for (NATIVE_INT_TYPE i = 0; i < b.getSize(); i++) {
      ASSERT_EQ(b.getData()[i], outbuf.getData()[i]);
    }
  }

  void Tester ::
    RMAPinTest()
  {
    U8 dat[1] = {0x01};

    RMAPPacket packet = RMAPPacket();
    packet.set(SpaceWireAddr(0x54, dat, 0, SpaceWireAddrType::LOGICAL),
               SpaceWireAddr(0x76, dat, 0, SpaceWireAddrType::LOGICAL),
               RMAPPacketType::WriteCommand, false, false, true, RMAPStatus::Success, (U8) 0x57, (U16) 0x05, (U8) 0x00, (U32) 0x1000, (U32) 0, Fw::Buffer(nullptr, 0, 0));

    this->invoke_to_RMAPin(0, packet);
    this->component.doDispatch();

    Fw::Buffer b = this->fromPortHistory_dataOut->at(0).fwBuffer;
    RMAPPacket out;

    RMAPPacket::decode(out, b);

    ASSERT_EQ(packet, out);
  }

  void Tester ::
    CCSDSinTest()
  {
    U8 dat[1] = {0x01};
    CCSDSPacket packet = CCSDSPacket();
    packet.set(SpaceWireAddr(0x54, dat, 0, SpaceWireAddrType::LOGICAL),
               0x01, false, false, 0x57, CCSDSSeqType::UnSegmented, 0x00, Fw::Buffer(nullptr, 0, 0));

    this->clearHistory();

    this->invoke_to_CCSDSin(0, packet);
    this->component.doDispatch();

    Fw::Buffer b = this->fromPortHistory_dataOut->at(0).fwBuffer;
    CCSDSPacket out;

    CCSDSPacket::decode(out, b);

    ASSERT_EQ(packet, out);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_CCSDSout_handler(
        const NATIVE_INT_TYPE portNum,
        const SpaceWire::CCSDSPacket &p
    )
  {
    this->pushFromPortEntry_CCSDSout(p);
  }

  void Tester ::
    from_RMAPout_handler(
        const NATIVE_INT_TYPE portNum,
        const SpaceWire::RMAPPacket &p
    )
  {
    this->pushFromPortEntry_RMAPout(p);
  }

  void Tester ::
    from_dataOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_dataOut(fwBuffer);
  }

  void Tester ::
    from_rawOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_rawOut(fwBuffer);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // CCSDSin
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_CCSDSin(
          i,
          this->component.get_CCSDSin_InputPort(i)
      );
    }

    // RMAPin
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_RMAPin(
          i,
          this->component.get_RMAPin_InputPort(i)
      );
    }

    // dataIn
    this->connect_to_dataIn(
        0,
        this->component.get_dataIn_InputPort(0)
    );

    // rawIn
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_rawIn(
          i,
          this->component.get_rawIn_InputPort(i)
      );
    }

    // registerAPID
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_registerAPID(
          i,
          this->component.get_registerAPID_InputPort(i)
      );
    }

    // registerProtocol
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_registerProtocol(
          i,
          this->component.get_registerProtocol_InputPort(i)
      );
    }

    // registerTransID
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->connect_to_registerTransID(
          i,
          this->component.get_registerTransID_InputPort(i)
      );
    }

    // CCSDSout
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->component.set_CCSDSout_OutputPort(
          i,
          this->get_from_CCSDSout(i)
      );
    }

    // RMAPout
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->component.set_RMAPout_OutputPort(
          i,
          this->get_from_RMAPout(i)
      );
    }

    // dataOut
    this->component.set_dataOut_OutputPort(
        0,
        this->get_from_dataOut(0)
    );

    // rawOut
    for (NATIVE_INT_TYPE i = 0; i < 32; ++i) {
      this->component.set_rawOut_OutputPort(
          i,
          this->get_from_rawOut(i)
      );
    }

  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace SpaceWire
