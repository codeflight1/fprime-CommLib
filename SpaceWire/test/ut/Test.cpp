#include <gtest/gtest.h>
#include <SpaceWire/RMAPPacket.cpp>
#include <SpaceWire/SpaceWireAddr.cpp>
#include <SpaceWire/SpaceWireAddrTypeEnumAc.cpp>
#include <Fw/Buffer/Buffer.cpp>

#include <cstdlib>

#include "EncodeTest.cpp"
#include "DecodeTest.cpp"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
