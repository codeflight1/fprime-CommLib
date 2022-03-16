#include <gtest/gtest.h>

#include <SpaceWire/RMAPPacket.hpp>
#include <SpaceWire/CCSDSPacket.hpp>

#include "RMAPEncodeTest.cpp"
#include "RMAPDecodeTest.cpp"
#include "CCSDSEncodeTest.cpp"
#include "CCSDSDecodeTest.cpp"

#include "Tester.hpp"

SpaceWire::Tester tester;

TEST(DecoderTest, RMAPDispatch) {
    tester.RMAPDispatchTest();
}

TEST(DecoderTest, CCSDSDispatch) {
    tester.CCSDSDispatchTest();
}

TEST(DecoderTest, rawDispatch) {
    tester.rawDispatchTest();
}

TEST(DecoderTest, RMAPin) {
    tester.RMAPinTest();
}

TEST(DecoderTest, CCSDSin) {
    tester.CCSDSinTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
