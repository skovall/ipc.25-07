#include <gtest/gtest.h>
#include "Utils.h"

using namespace testing;

TEST(IPv4ValidationTest, ValidIPv4Addresses) {
    EXPECT_TRUE(Utils::VlIPv4("0.0.0.0"));
    EXPECT_TRUE(Utils::VlIPv4("8.8.8.8"));
    EXPECT_TRUE(Utils::VlIPv4("192.168.1.1"));
    EXPECT_TRUE(Utils::VlIPv4("255.255.255.255"));
}

TEST(IPv4ValidationTest, InvalidIPv4Addresses) {
    EXPECT_FALSE(Utils::VlIPv4(""));
    EXPECT_FALSE(Utils::VlIPv4("256.1.1.1"));
    EXPECT_FALSE(Utils::VlIPv4("192.168.1"));
    EXPECT_FALSE(Utils::VlIPv4("192.168.1.1.1"));
}

TEST(IPv6ValidationTest, ValidIPv6Addresses) {
    EXPECT_TRUE(Utils::VlIPv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
    EXPECT_TRUE(Utils::VlIPv6("::1"));
    EXPECT_TRUE(Utils::VlIPv6("fe80::1"));
}

TEST(IPv6ValidationTest, InvalidIPv6Addresses) {
    EXPECT_FALSE(Utils::VlIPv6(""));
    EXPECT_FALSE(Utils::VlIPv6("invalid"));
    EXPECT_FALSE(Utils::VlIPv6("gggg::1"));
}

TEST(IPValidationTest, ValidIPAddresses) {
    EXPECT_TRUE(Utils::VlIP("8.8.8.8"));
    EXPECT_TRUE(Utils::VlIP("::1"));
    EXPECT_TRUE(Utils::VlIP("2001:0db8::1"));
}

TEST(IPValidationTest, InvalidIPAddresses) {
    EXPECT_FALSE(Utils::VlIP(""));
    EXPECT_FALSE(Utils::VlIP("invalid"));
    EXPECT_FALSE(Utils::VlIP("256.1.1.1"));
}
