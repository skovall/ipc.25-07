#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include "Utils.h"

using namespace testing;

// Тесты для VlIPv4

class IPv4ValidationTest : public ::testing::Test {
protected:
    bool validateIPv4(const std::string& ip) {
        return Utils::VlIPv4(ip);
    }
};

TEST_F(IPv4ValidationTest, ValidIPv4Addresses) {
    // Стандартные валидные IPv4 адреса
    EXPECT_TRUE(validateIPv4("0.0.0.0"));
    EXPECT_TRUE(validateIPv4("1.1.1.1"));
    EXPECT_TRUE(validateIPv4("8.8.8.8"));
    EXPECT_TRUE(validateIPv4("192.168.1.1"));
    EXPECT_TRUE(validateIPv4("255.255.255.255"));
    EXPECT_TRUE(validateIPv4("10.0.0.1"));
    EXPECT_TRUE(validateIPv4("172.16.0.1"));
    EXPECT_TRUE(validateIPv4("127.0.0.1"));
    EXPECT_TRUE(validateIPv4("192.0.2.0"));
    EXPECT_TRUE(validateIPv4("203.0.113.0"));
    EXPECT_TRUE(validateIPv4("198.51.100.0"));
}

TEST_F(IPv4ValidationTest, ValidIPv4WithLeadingZeros) {
    // IPv4 с ведущими нулями (должны быть валидными)
    EXPECT_TRUE(validateIPv4("001.002.003.004"));
    EXPECT_TRUE(validateIPv4("000.000.000.000"));
    EXPECT_TRUE(validateIPv4("192.168.001.001"));
}

TEST_F(IPv4ValidationTest, ValidIPv4BoundaryValues) {
    // Граничные значения
    EXPECT_TRUE(validateIPv4("0.0.0.0"));
    EXPECT_TRUE(validateIPv4("255.255.255.255"));
    EXPECT_TRUE(validateIPv4("1.2.3.4"));
    EXPECT_TRUE(validateIPv4("0.255.0.255"));
}

TEST_F(IPv4ValidationTest, InvalidIPv4Addresses) {
    // Невалидные IPv4 адреса
    EXPECT_FALSE(validateIPv4(""));
    EXPECT_FALSE(validateIPv4(" "));
    EXPECT_FALSE(validateIPv4("256.1.1.1"));
    EXPECT_FALSE(validateIPv4("192.168.1.256"));
    EXPECT_FALSE(validateIPv4("192.168.1"));
    EXPECT_FALSE(validateIPv4("192.168.1.1.1"));
    EXPECT_FALSE(validateIPv4("a.b.c.d"));
    EXPECT_FALSE(validateIPv4("192.168.1.-1"));
    EXPECT_FALSE(validateIPv4("192.168.1.1.1"));
    EXPECT_FALSE(validateIPv4("..."));
    EXPECT_FALSE(validateIPv4("1.2.3"));
    EXPECT_FALSE(validateIPv4("1.2.3.4.5"));
    EXPECT_FALSE(validateIPv4("1.2.3.256"));
    EXPECT_FALSE(validateIPv4("1.2.3.04.5"));
    EXPECT_FALSE(validateIPv4("01.02.03.04.05"));
}

TEST_F(IPv4ValidationTest, InvalidIPv4WithOutOfRangeValues) {
    // Значения вне диапазона 0-255
    EXPECT_FALSE(validateIPv4("256.256.256.256"));
    EXPECT_FALSE(validateIPv4("1000.1.1.1"));
    EXPECT_FALSE(validateIPv4("1.1000.1.1"));
    EXPECT_FALSE(validateIPv4("1.1.1000.1"));
    EXPECT_FALSE(validateIPv4("1.1.1.1000"));
    EXPECT_FALSE(validateIPv4("-1.1.1.1"));
    EXPECT_FALSE(validateIPv4("1.-1.1.1"));
    EXPECT_FALSE(validateIPv4("1.1.-1.1"));
    EXPECT_FALSE(validateIPv4("1.1.1.-1"));
}

TEST_F(IPv4ValidationTest, InvalidIPv4WithLetters) {
    // С буквами и спецсимволами
    EXPECT_FALSE(validateIPv4("abc.def.ghi.jkl"));
    EXPECT_FALSE(validateIPv4("1.2.3.4a"));
    EXPECT_FALSE(validateIPv4("a1.2.3.4"));
    EXPECT_FALSE(validateIPv4("1.2.3.4.5a"));
    EXPECT_FALSE(validateIPv4("1.2.3.4."));
    EXPECT_FALSE(validateIPv4(".1.2.3.4"));
    EXPECT_FALSE(validateIPv4("1..2.3.4"));
    EXPECT_FALSE(validateIPv4("1.2..3.4"));
    EXPECT_FALSE(validateIPv4("1.2.3..4"));
}

TEST_F(IPv4ValidationTest, IPv4WithSpacesAndTabs) {
    // С пробелами и табуляцией
    EXPECT_FALSE(validateIPv4(" 192.168.1.1"));
    EXPECT_FALSE(validateIPv4("192.168.1.1 "));
    EXPECT_FALSE(validateIPv4("192.168.1.1\t"));
    EXPECT_FALSE(validateIPv4("\t192.168.1.1"));
    EXPECT_FALSE(validateIPv4("192.168. 1.1"));
    EXPECT_FALSE(validateIPv4("192. 168.1.1"));
}

TEST_F(IPv4ValidationTest, IPv4WithNewlines) {
    // С символами новой строки
    EXPECT_FALSE(validateIPv4("192.168.1.1\n"));
    EXPECT_FALSE(validateIPv4("\n192.168.1.1"));
    EXPECT_FALSE(validateIPv4("192.168.1.1\r"));
    EXPECT_FALSE(validateIPv4("\r192.168.1.1"));
}

// Тесты для VlIPv6

class IPv6ValidationTest : public ::testing::Test {
protected:
    bool validateIPv6(const std::string& ip) {
        return Utils::VlIPv6(ip);
    }
};

TEST_F(IPv6ValidationTest, ValidIPv6Addresses_FullForm) {
    // Полные IPv6 адреса
    EXPECT_TRUE(validateIPv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
    EXPECT_TRUE(validateIPv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
    EXPECT_TRUE(validateIPv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
}

TEST_F(IPv6ValidationTest, ValidIPv6Addresses_CompressedForm) {
    // Сжатые IPv6 адреса
    EXPECT_TRUE(validateIPv6("2001:db8:85a3::8a2e:370:7334"));
    EXPECT_TRUE(validateIPv6("::1"));
    EXPECT_TRUE(validateIPv6("::"));
    EXPECT_TRUE(validateIPv6("fe80::1ff:fe23:4567:890a"));
    EXPECT_TRUE(validateIPv6("2001::1"));
    EXPECT_TRUE(validateIPv6("2001:0db8::0001"));
    EXPECT_TRUE(validateIPv6("::ffff:192.0.2.1"));
    EXPECT_TRUE(validateIPv6("2001:db8::"));
    EXPECT_TRUE(validateIPv6("::1234:5678"));
    EXPECT_TRUE(validateIPv6("abcd::1234"));
    EXPECT_TRUE(validateIPv6("1::"));
    EXPECT_TRUE(validateIPv6("::2"));
    EXPECT_TRUE(validateIPv6("1::2"));
    EXPECT_TRUE(validateIPv6("a:b:c:d:e:f:g:h"));
}

TEST_F(IPv6ValidationTest, ValidIPv6Addresses_LoopbackAndSpecial) {
    // Специальные IPv6 адреса
    EXPECT_TRUE(validateIPv6("::1"));  // loopback
    EXPECT_TRUE(validateIPv6("::"));   // unspecified
    EXPECT_TRUE(validateIPv6("fe80::")); // link-local
    EXPECT_TRUE(validateIPv6("ff02::1")); // multicast
}

TEST_F(IPv6ValidationTest, ValidIPv6Addresses_UppercaseAndLowercase) {
    // Разный регистр
    EXPECT_TRUE(validateIPv6("2001:0DB8:85A3:0000:0000:8A2E:0370:7334"));
    EXPECT_TRUE(validateIPv6("2001:Db8:85a3::8A2e:370:7334"));
    EXPECT_TRUE(validateIPv6("2001:0db8:85a3::8A2E:0370:7334"));
}

TEST_F(IPv6ValidationTest, InvalidIPv6Addresses) {
    // Невалидные IPv6 адреса
    EXPECT_FALSE(validateIPv6(""));
    EXPECT_FALSE(validateIPv6(" "));
    EXPECT_FALSE(validateIPv6("::1:2:3:4:5:6:7:8")); // слишком много блоков
    EXPECT_FALSE(validateIPv6("12345::1")); // блок слишком длинный
    EXPECT_FALSE(validateIPv6("2001:0db8:85a3::8a2e:0370:7334:1234")); // слишком много
    EXPECT_FALSE(validateIPv6("2001:0db8:85a3::8a2e:0370:7334:")); // заканчивается на :
    EXPECT_FALSE(validateIPv6(":2001:0db8::1")); // начинается с :
    EXPECT_FALSE(validateIPv6("2001:0db8:::")); // несколько ::
    EXPECT_FALSE(validateIPv6("2001::0db8::1")); // дважды ::
    EXPECT_FALSE(validateIPv6("not an ip"));
    EXPECT_FALSE(validateIPv6("gggg::1")); // неверные символы
    EXPECT_FALSE(validateIPv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334:1234")); // слишком длинный
    EXPECT_FALSE(validateIPv6("2001:0db8:85a3:0000:0000:8a2e:0370")); // слишком короткий
}

TEST_F(IPv6ValidationTest, InvalidIPv6WithIPv4Mixing) {
    // Неправильное смешивание с IPv4
    EXPECT_FALSE(validateIPv6("::ffff:192.168.1.256")); // IPv4 часть невалидна
    EXPECT_FALSE(validateIPv6("::ffff:192.168.1")); // неполный IPv4
    EXPECT_FALSE(validateIPv6("::ffff:192.168.1.1.1")); // слишком много
}

TEST_F(IPv6ValidationTest, InvalidIPv6WithSpaces) {
    // С пробелами
    EXPECT_FALSE(validateIPv6(" 2001:0db8::1"));
    EXPECT_FALSE(validateIPv6("2001:0db8::1 "));
    EXPECT_FALSE(validateIPv6("2001:0db8: :1"));
}

// Тесты для VlIP (общая валидация)

class IPValidationTest : public ::testing::Test {
protected:
    bool validateIP(const std::string& ip) {
        return Utils::VlIP(ip);
    }
};

TEST_F(IPValidationTest, ValidIPv4Addresses) {
    EXPECT_TRUE(validateIP("0.0.0.0"));
    EXPECT_TRUE(validateIP("8.8.8.8"));
    EXPECT_TRUE(validateIP("192.168.1.1"));
    EXPECT_TRUE(validateIP("255.255.255.255"));
    EXPECT_TRUE(validateIP("10.0.0.1"));
    EXPECT_TRUE(validateIP("127.0.0.1"));
}

TEST_F(IPValidationTest, ValidIPv6Addresses) {
    EXPECT_TRUE(validateIP("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
    EXPECT_TRUE(validateIP("::1"));
    EXPECT_TRUE(validateIP("fe80::1ff:fe23:4567:890a"));
    EXPECT_TRUE(validateIP("2001::1"));
}

TEST_F(IPValidationTest, InvalidIPAddresses) {
    EXPECT_FALSE(validateIP(""));
    EXPECT_FALSE(validateIP(" "));
    EXPECT_FALSE(validateIP("invalid"));
    EXPECT_FALSE(validateIP("256.1.1.1"));
    EXPECT_FALSE(validateIP("192.168.1"));
    EXPECT_FALSE(validateIP("192.168.1.1.1"));
    EXPECT_FALSE(validateIP("gggg::1"));
    EXPECT_FALSE(validateIP("not an ip"));
}

TEST_F(IPValidationTest, MixedAndSpecialCases) {
    // IP версии 4 с ведущими нулями (должны быть валидными)
    EXPECT_TRUE(validateIP("001.002.003.004"));
    
    // Пустые строки
    EXPECT_FALSE(validateIP(""));
    EXPECT_FALSE(validateIP(std::string()));
    
    // Только пробелы
    EXPECT_FALSE(validateIP("   "));
    EXPECT_FALSE(validateIP("\t\n"));
}

// Тесты на производительность

class IPValidationPerformanceTest : public ::testing::Test {};

TEST_F(IPValidationPerformanceTest, ValidateIPv4_Performance) {
    const int iterations = 10000;
    const std::string ip = "192.168.1.1";
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        Utils::VlIPv4(ip);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    auto avg_us = duration.count() / iterations;
    
    EXPECT_LT(avg_us, 100) << "Average validation time: " << avg_us << "us";
}

TEST_F(IPValidationPerformanceTest, ValidateIPv6_Performance) {
    const int iterations = 10000;
    const std::string ip = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        Utils::VlIPv6(ip);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    auto avg_us = duration.count() / iterations;
    
    EXPECT_LT(avg_us, 200) << "Average validation time: " << avg_us << "us";
}

// Параметризованные тесты

class IPv4ParameterizedTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {};

TEST_P(IPv4ParameterizedTest, ValidateIPv4) {
    auto [ip, expected] = GetParam();
    EXPECT_EQ(Utils::VlIPv4(ip), expected) << "Failed for IP: " << ip;
}

INSTANTIATE_TEST_SUITE_P(
    IPv4TestCases,
    IPv4ParameterizedTest,
    Values(
        // Валидные IPv4
        std::make_tuple("0.0.0.0", true),
        std::make_tuple("1.1.1.1", true),
        std::make_tuple("8.8.8.8", true),
        std::make_tuple("192.168.1.1", true),
        std::make_tuple("255.255.255.255", true),
        std::make_tuple("10.0.0.1", true),
        std::make_tuple("172.16.0.1", true),
        std::make_tuple("127.0.0.1", true),
        
        // Валидные с ведущими нулями
        std::make_tuple("001.002.003.004", true),
        std::make_tuple("000.000.000.000", true),
        std::make_tuple("192.168.001.001", true),
        
        // Невалидные
        std::make_tuple("", false),
        std::make_tuple("256.1.1.1", false),
        std::make_tuple("192.168.1.256", false),
        std::make_tuple("192.168.1", false),
        std::make_tuple("192.168.1.1.1", false),
        std::make_tuple("abc.def.ghi.jkl", false),
        std::make_tuple("1.2.3", false),
        std::make_tuple("1.2.3.4.5", false),
        std::make_tuple("1.2.3.04.5", false),
        std::make_tuple("1000.1.1.1", false),
        std::make_tuple("1.1000.1.1", false),
        std::make_tuple("1.1.1000.1", false),
        std::make_tuple("1.1.1.1000", false),
        std::make_tuple("-1.1.1.1", false),
        std::make_tuple("1.-1.1.1", false),
        std::make_tuple("1.1.-1.1", false),
        std::make_tuple("1.1.1.-1", false),
        std::make_tuple(" 192.168.1.1", false),
        std::make_tuple("192.168.1.1 ", false),
        std::make_tuple("192.168. 1.1", false)
    )
);

class IPv6ParameterizedTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {};

TEST_P(IPv6ParameterizedTest, ValidateIPv6) {
    auto [ip, expected] = GetParam();
    EXPECT_EQ(Utils::VlIPv6(ip), expected) << "Failed for IP: " << ip;
}

INSTANTIATE_TEST_SUITE_P(
    IPv6TestCases,
    IPv6ParameterizedTest,
    Values(
        // Валидные IPv6 (полная форма)
        std::make_tuple("2001:0db8:85a3:0000:0000:8a2e:0370:7334", true),
        
        // Валидные IPv6 (сжатая форма)
        std::make_tuple("2001:db8:85a3::8a2e:370:7334", true),
        std::make_tuple("::1", true),
        std::make_tuple("::", true),
        std::make_tuple("fe80::1ff:fe23:4567:890a", true),
        std::make_tuple("2001::1", true),
        std::make_tuple("2001:0db8::0001", true),
        std::make_tuple("::ffff:192.0.2.1", true),
        std::make_tuple("2001:db8::", true),
        std::make_tuple("::1234:5678", true),
        std::make_tuple("abcd::1234", true),
        std::make_tuple("1::", true),
        std::make_tuple("::2", true),
        std::make_tuple("1::2", true),
        
        // Валидные с разным регистром
        std::make_tuple("2001:0DB8:85A3:0000:0000:8A2E:0370:7334", true),
        std::make_tuple("2001:Db8:85a3::8A2e:370:7334", true),
        
        // Невалидные IPv6
        std::make_tuple("", false),
        std::make_tuple(" ", false),
        std::make_tuple("::1:2:3:4:5:6:7:8", false),
        std::make_tuple("12345::1", false),
        std::make_tuple("2001:0db8:85a3::8a2e:0370:7334:1234", false),
        std::make_tuple("2001:0db8:85a3::8a2e:0370:7334:", false),
        std::make_tuple(":2001:0db8::1", false),
        std::make_tuple("2001:0db8:::", false),
        std::make_tuple("2001::0db8::1", false),
        std::make_tuple("not an ip", false),
        std::make_tuple("gggg::1", false),
        std::make_tuple("2001:0db8:85a3:0000:0000:8a2e:0370:7334:1234", false),
        std::make_tuple("2001:0db8:85a3:0000:0000:8a2e:0370", false),
        std::make_tuple(" 2001:0db8::1", false),
        std::make_tuple("2001:0db8::1 ", false)
    )
);

class IPParameterizedTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {};

TEST_P(IPParameterizedTest, ValidateIP) {
    auto [ip, expected] = GetParam();
    EXPECT_EQ(Utils::VlIP(ip), expected) << "Failed for IP: " << ip;
}

INSTANTIATE_TEST_SUITE_P(
    AllIPTests,
    IPParameterizedTest,
    Values(
        // Валидные IPv4
        std::make_tuple("8.8.8.8", true),
        std::make_tuple("192.168.1.1", true),
        std::make_tuple("255.255.255.255", true),
        std::make_tuple("0.0.0.0", true),
        
        // Валидные IPv6
        std::make_tuple("::1", true),
        std::make_tuple("2001:0db8:85a3::8a2e:0370:7334", true),
        std::make_tuple("fe80::1", true),
        
        // Невалидные
        std::make_tuple("", false),
        std::make_tuple("invalid", false),
        std::make_tuple("256.1.1.1", false),
        std::make_tuple("192.168.1", false),
        std::make_tuple("gggg::1", false),
        std::make_tuple("not an ip", false),
        std::make_tuple("192.168.1.1.1", false),
        std::make_tuple("2001:0db8::85a3::8a2e", false)
    )
);

// Тесты для std::string_view

class StringViewTest : public ::testing::Test {};

TEST_F(StringViewTest, ValidIPv4_WithStringView) {
    std::string ip = "192.168.1.1";
    std::string_view view(ip);
    
    EXPECT_TRUE(Utils::VlIPv4(view));
    EXPECT_TRUE(Utils::VlIP(view));
    EXPECT_FALSE(Utils::VlIPv6(view));
}

TEST_F(StringViewTest, ValidIPv6_WithStringView) {
    std::string ip = "2001:0db8::1";
    std::string_view view(ip);
    
    EXPECT_TRUE(Utils::VlIPv6(view));
    EXPECT_TRUE(Utils::VlIP(view));
    EXPECT_FALSE(Utils::VlIPv4(view));
}

TEST_F(StringViewTest, Substring_Validation) {
    std::string full = "IP: 192.168.1.1:8080";
    std::string_view ip_part(full.c_str() + 4, 11); // "192.168.1.1"
    
    EXPECT_TRUE(Utils::VlIPv4(ip_part));
}

// Тесты на noexcept спецификацию

class NoExceptTest : public ::testing::Test {};

TEST_F(NoExceptTest, VlIPv4_IsNoExcept) {
    EXPECT_TRUE(noexcept(Utils::VlIPv4("192.168.1.1")));
    EXPECT_TRUE(noexcept(Utils::VlIPv4(std::string_view())));
}

TEST_F(NoExceptTest, VlIPv6_IsNoExcept) {
    EXPECT_TRUE(noexcept(Utils::VlIPv6("::1")));
    EXPECT_TRUE(noexcept(Utils::VlIPv6(std::string_view())));
}

TEST_F(NoExceptTest, VlIP_IsNoExcept) {
    EXPECT_TRUE(noexcept(Utils::VlIP("8.8.8.8")));
    EXPECT_TRUE(noexcept(Utils::VlIP(std::string_view())));
}

// Тесты на корректность работы с памятью

class MemorySafetyTest : public ::testing::Test {};

TEST_F(MemorySafetyTest, VeryLongString_DoesNotCrash) {
    std::string long_ip(1000000, 'a');
    EXPECT_FALSE(Utils::VlIP(long_ip));
    EXPECT_FALSE(Utils::VlIPv4(long_ip));
    EXPECT_FALSE(Utils::VlIPv6(long_ip));
}

TEST_F(MemorySafetyTest, EmptyString_HandlesGracefully) {
    std::string empty;
    EXPECT_FALSE(Utils::VlIP(empty));
    EXPECT_FALSE(Utils::VlIPv4(empty));
    EXPECT_FALSE(Utils::VlIPv6(empty));
}

TEST_F(MemorySafetyTest, NullCharacters_InString) {
    std::string ip_with_null = "192.168.1.1";
    ip_with_null[5] = '\0';
    
    // Поведение может быть неопределенным, но функция не должна крашиться
    EXPECT_NO_THROW(Utils::VlIP(ip_with_null));
}

// Тесты для краевых случаев

class EdgeCasesTest : public ::testing::Test {};

TEST_F(EdgeCasesTest, IPv4_MaximumValues) {
    EXPECT_TRUE(Utils::VlIPv4("255.255.255.255"));
    EXPECT_FALSE(Utils::VlIPv4("255.255.255.256"));
    EXPECT_TRUE(Utils::VlIPv4("0.0.0.0"));
}

TEST_F(EdgeCasesTest, IPv6_MaximumCompression) {
    EXPECT_TRUE(Utils::VlIPv6("::"));
    EXPECT_TRUE(Utils::VlIPv6("::1"));
    EXPECT_TRUE(Utils::VlIPv6("1::"));
}

TEST_F(EdgeCasesTest, MixedCaseIPv6) {
    EXPECT_TRUE(Utils::VlIPv6("FE80:0000:0000:0000:0000:0000:0000:0001"));
    EXPECT_TRUE(Utils::VlIPv6("fe80::1"));
    EXPECT_TRUE(Utils::VlIPv6("FE80::1"));
}

// Бенчмарк тесты (отключены по умолчанию)

class BenchmarkTest : public ::testing::Test {};

TEST_F(BenchmarkTest, DISABLED_ValidateIPv4_Benchmark) {
    std::vector<std::string> ips = {
        "192.168.1.1", "8.8.8.8", "1.1.1.1", "255.255.255.255",
        "invalid", "256.1.1.1", "192.168.1", "192.168.1.1.1"
    };
    
    const int iterations = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (const auto& ip : ips) {
            Utils::VlIPv4(ip);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "IPv4 validation benchmark: " << duration.count() << "ms for "
              << iterations * ips.size() << " validations" << std::endl;
}

TEST_F(BenchmarkTest, DISABLED_ValidateIPv6_Benchmark) {
    std::vector<std::string> ips = {
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
        "::1",
        "fe80::1ff:fe23:4567:890a",
        "2001::1",
        "invalid",
        "gggg::1",
        "2001:0db8::1::1"
    };
    
    const int iterations = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (const auto& ip : ips) {
            Utils::VlIPv6(ip);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "IPv6 validation benchmark: " << duration.count() << "ms for "
              << iterations * ips.size() << " validations" << std::endl;
}

// Main function

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    return RUN_ALL_TESTS();
}
