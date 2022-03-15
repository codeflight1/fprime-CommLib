#include <gtest/gtest.h>
#include <SpaceWire/RMAPPacket.cpp>
#include <SpaceWire/CCSDSPacket.cpp>
#include <SpaceWire/SpaceWireAddr.cpp>
#include <SpaceWire/SpaceWireAddrTypeEnumAc.cpp>
#include <SpaceWire/CCSDSSeqTypeEnumAc.cpp>
#include <Fw/Buffer/Buffer.cpp>

#include <cstdlib>

#include "RMAPEncodeTest.cpp"
#include "RMAPDecodeTest.cpp"
#include "CCSDSEncodeTest.cpp"
#include "CCSDSDecodeTest.cpp"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
