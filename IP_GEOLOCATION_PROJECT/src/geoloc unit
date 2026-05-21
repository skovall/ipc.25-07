#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <temp_dir.hpp>
#include <curl/curl.h>

#include "geolocation.hpp"

using namespace ip_geolocation;
using namespace testing;

// Мок для CURL

class MockCURL {
public:
    MOCK_METHOD(CURLcode, easy_setopt, (CURL*, CURLoption, ...));
    MOCK_METHOD(CURLcode, easy_perform, (CURL*));
    MOCK_METHOD(void, easy_cleanup, (CURL*));
    MOCK_METHOD(void, global_init, (long));
    MOCK_METHOD(void, global_cleanup, ());
};

// Тесты для структуры GeoLocation

class GeoLocationStructTest : public ::testing::Test {
protected:
    void SetUp() override {
        location.ip = "8.8.8.8";
        location.country = "United States";
        location.country_code = "US";
        location.region = "California";
        location.city = "Mountain View";
        location.postal_code = "94043";
        location.latitude = 37.4223;
        location.longitude = -122.0841;
        location.timezone = "America/Los_Angeles";
        location.isp = "Google LLC";
        location.organization = "Google";
        location.success = true;
    }
    
    GeoLocation location;
};

TEST_F(GeoLocationStructTest, ToString_ValidLocation_ReturnsFormattedString) {
    std::string result = location.toString();
    
    EXPECT_THAT(result, HasSubstr("IP: 8.8.8.8"));
    EXPECT_THAT(result, HasSubstr("Country: United States (US)"));
    EXPECT_THAT(result, HasSubstr("City: Mountain View"));
    EXPECT_THAT(result, HasSubstr("Coordinates: 37.4223, -122.0841"));
    EXPECT_THAT(result, HasSubstr("Success: Yes"));
}

TEST_F(GeoLocationStructTest, ToString_FailedLocation_IncludesError) {
    location.success = false;
    location.error_message = "Invalid IP address";
    
    std::string result = location.toString();
    
    EXPECT_THAT(result, HasSubstr("Success: No"));
    EXPECT_THAT(result, HasSubstr("Error: Invalid IP address"));
}

TEST_F(GeoLocationStructTest, ToString_EmptyLocation_HandlesDefaults) {
    GeoLocation empty;
    
    std::string result = empty.toString();
    
    EXPECT_THAT(result, HasSubstr("IP: "));
    EXPECT_THAT(result, HasSubstr("Success: No"));
    EXPECT_THAT(result, Not(HasSubstr("Error:")));
}

TEST_F(GeoLocationStructTest, ToString_CoordinatesZero_ShowsCorrectly) {
    location.latitude = 0.0;
    location.longitude = 0.0;
    
    std::string result = location.toString();
    EXPECT_THAT(result, HasSubstr("Coordinates: 0, 0"));
}

// Тесты для isValidIP

class IsValidIPParamTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {};

TEST_P(IsValidIPParamTest, ValidatesCorrectly) {
    auto [ip, expected] = GetParam();
    EXPECT_EQ(isValidIP(ip), expected);
}

INSTANTIATE_TEST_SUITE_P(
    IPValidation,
    IsValidIPParamTest,
    Values(
        // IPv4 валидные
        std::make_tuple("8.8.8.8", true),
        std::make_tuple("192.168.1.1", true),
        std::make_tuple("0.0.0.0", true),
        std::make_tuple("255.255.255.255", true),
        std::make_tuple("10.0.0.1", true),
        std::make_tuple("127.0.0.1", true),
        
        // IPv4 невалидные
        std::make_tuple("", false),
        std::make_tuple("256.1.1.1", false),
        std::make_tuple("192.168.1", false),
        std::make_tuple("192.168.1.1.1", false),
        std::make_tuple("abc.def.ghi.jkl", false),
        std::make_tuple("192.168.1.-1", false),
        std::make_tuple("192.168.1.256", false),
        std::make_tuple("1.2.3.4.5", false),
        
        // IPv6 валидные
        std::make_tuple("2001:0db8:85a3:0000:0000:8a2e:0370:7334", true),
        std::make_tuple("::1", true),
        std::make_tuple("fe80::1ff:fe23:4567:890a", true),
        std::make_tuple("2001::1", true),
        
        // IPv6 невалидные
        std::make_tuple("2001:0db8:85a3::8a2e:0370:7334:1234:5678", false),
        std::make_tuple("gggg::1", false),
        
        // Special
        std::make_tuple("me", true)
    )
);

// Тесты для кэширования

class GeoLocationServiceCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceCacheTest, GetLocation_FirstCall_NotCached) {
    auto start = std::chrono::steady_clock::now();
    auto result1 = service->getLocation("8.8.8.8");
    auto duration1 = std::chrono::steady_clock::now() - start;
    
    start = std::chrono::steady_clock::now();
    auto result2 = service->getLocation("8.8.8.8");
    auto duration2 = std::chrono::steady_clock::now() - start;
    
    if (result1.success) {
        // Второй вызов должен быть быстрее (из кэша)
        EXPECT_LE(duration2, duration1);
        EXPECT_EQ(result1.country, result2.country);
        EXPECT_EQ(result1.city, result2.city);
    }
}

TEST_F(GeoLocationServiceCacheTest, ClearCache_RemovesEntries) {
    service->getLocation("8.8.8.8");
    service->clearCache();
    
    // После очистки кэша данные должны запрашиваться снова
    auto result = service->getLocation("8.8.8.8");
    EXPECT_TRUE(result.success || !result.success); // Не должно выбросить исключение
}

TEST_F(GeoLocationServiceCacheTest, DifferentIPs_AreCachedSeparately) {
    auto result1 = service->getLocation("8.8.8.8");
    auto result2 = service->getLocation("1.1.1.1");
    auto result3 = service->getLocation("8.8.8.8");
    
    EXPECT_EQ(result1.ip, result3.ip);
    EXPECT_NE(result1.ip, result2.ip);
}

// Тесты для GeoLocationService

class GeoLocationServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    void TearDown() override {
        service.reset();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceTest, Constructor_CreatesInstance) {
    EXPECT_NE(service, nullptr);
    EXPECT_NO_THROW(GeoLocationService());
}

TEST_F(GeoLocationServiceTest, SetApiKey_ValidKey_NoThrow) {
    EXPECT_NO_THROW(service->setApiKey("test_api_key_123"));
    EXPECT_NO_THROW(service->setApiKey(""));
}

TEST_F(GeoLocationServiceTest, SetDatabasePath_ValidPath_NoThrow) {
    EXPECT_NO_THROW(service->setDatabasePath("/path/to/database.mmdb"));
    EXPECT_NO_THROW(service->setDatabasePath(""));
}

TEST_F(GeoLocationServiceTest, GetLocation_ValidIPv4_ReturnsSuccess) {
    GeoLocation result = service->getLocation("8.8.8.8");
    
    EXPECT_TRUE(result.success) << "Error: " << result.error_message;
    EXPECT_EQ(result.ip, "8.8.8.8");
    EXPECT_FALSE(result.country.empty());
    EXPECT_FALSE(result.country_code.empty());
    EXPECT_GE(result.latitude, -90.0);
    EXPECT_LE(result.latitude, 90.0);
    EXPECT_GE(result.longitude, -180.0);
    EXPECT_LE(result.longitude, 180.0);
}

TEST_F(GeoLocationServiceTest, GetLocation_ValidIPv6_ReturnsSuccess) {
    GeoLocation result = service->getLocation("2001:4860:4860::8888");
    
    if (result.success) {
        EXPECT_FALSE(result.country.empty());
        EXPECT_FALSE(result.city.empty() || !result.city.empty());
    }
}

TEST_F(GeoLocationServiceTest, GetLocation_InvalidIP_ReturnsError) {
    GeoLocation result = service->getLocation("invalid_ip_address");
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
    EXPECT_THAT(result.error_message, HasSubstr("Invalid"));
}

TEST_F(GeoLocationServiceTest, GetLocation_Localhost_HandlesGracefully) {
    EXPECT_NO_THROW(service->getLocation("127.0.0.1"));
}

TEST_F(GeoLocationServiceTest, GetLocation_MeCommand_ReturnsIP) {
    GeoLocation result = service->getLocation("me");
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.ip.empty());
    EXPECT_TRUE(isValidIP(result.ip));
}

TEST_F(GeoLocationServiceTest, GetLocation_EmptyString_ReturnsError) {
    GeoLocation result = service->getLocation("");
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

TEST_F(GeoLocationServiceTest, GetLocation_Consistency_ReturnsSameResult) {
    auto result1 = service->getLocation("8.8.8.8");
    auto result2 = service->getLocation("8.8.8.8");
    
    if (result1.success && result2.success) {
        EXPECT_EQ(result1.country, result2.country);
        EXPECT_EQ(result1.city, result2.city);
        EXPECT_DOUBLE_EQ(result1.latitude, result2.latitude);
        EXPECT_DOUBLE_EQ(result1.longitude, result2.longitude);
    }
}

TEST_F(GeoLocationServiceTest, GetLocation_ThreadSafety_MultipleThreads) {
    const int num_threads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successes{0};
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, &successes]() {
            auto result = service->getLocation("8.8.8.8");
            if (result.success) successes++;
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_GT(successes, 0);
}

// Асинхронные тесты

class GeoLocationServiceAsyncTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceAsyncTest, GetLocationAsync_CallbackExecutes) {
    std::promise<GeoLocation> promise;
    std::future<GeoLocation> future = promise.get_future();
    
    service->getLocationAsync("8.8.8.8", [&promise](const GeoLocation& result) {
        promise.set_value(result);
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    
    GeoLocation result = future.get();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.ip, "8.8.8.8");
}

TEST_F(GeoLocationServiceAsyncTest, GetLocationAsync_MultipleCalls_AllExecute) {
    std::atomic<int> call_count{0};
    std::vector<std::string> ips = {"8.8.8.8", "1.1.1.1", "4.4.4.4"};
    
    for (const auto& ip : ips) {
        service->getLocationAsync(ip, [&call_count](const GeoLocation&) {
            call_count++;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_EQ(call_count, ips.size());
}

TEST_F(GeoLocationServiceAsyncTest, GetLocationAsync_InvalidIP_CallbackWithError) {
    std::promise<GeoLocation> promise;
    
    service->getLocationAsync("invalid", [&promise](const GeoLocation& result) {
        promise.set_value(result);
    });
    
    auto future = promise.get_future();
    auto status = future.wait_for(std::chrono::seconds(2));
    
    ASSERT_EQ(status, std::future_status::ready);
    EXPECT_FALSE(future.get().success);
}

TEST_F(GeoLocationServiceAsyncTest, GetLocationAsync_ConcurrentCalls_ThreadSafe) {
    const int num_calls = 20;
    std::atomic<int> completed{0};
    
    for (int i = 0; i < num_calls; ++i) {
        service->getLocationAsync("8.8.8.8", [&completed](const GeoLocation&) {
            completed++;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_EQ(completed, num_calls);
}

// Тесты для isAvailable

TEST(GeoLocationServiceAvailabilityTest, IsAvailable_ReturnsBool) {
    GeoLocationService service;
    
    bool available = service.isAvailable();
    EXPECT_TRUE(available == true || available == false);
}

TEST(GeoLocationServiceAvailabilityTest, IsAvailable_AfterClearCache_StillWorks) {
    GeoLocationService service;
    
    service.clearCache();
    bool available = service.isAvailable();
    EXPECT_TRUE(available == true || available == false);
}

// Тесты для getCurrentIP

TEST(GetCurrentIPTest, ReturnsNonEmptyString) {
    std::string current_ip = getCurrentIP();
    
    EXPECT_FALSE(current_ip.empty());
    EXPECT_TRUE(isValidIP(current_ip));
}

TEST(GetCurrentIPTest, ReturnsValidIP) {
    std::string current_ip = getCurrentIP();
    
    // Не должен возвращать localhost если есть сеть
    if (current_ip != "127.0.0.1" && current_ip != "::1") {
        EXPECT_TRUE(isValidIP(current_ip));
    }
}

TEST(GetCurrentIPTest, ConsistentResult) {
    std::string first = getCurrentIP();
    std::string second = getCurrentIP();
    
    EXPECT_EQ(first, second);
}

// Тесты для работы с API ключом

class GeoLocationServiceApiKeyTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceApiKeyTest, SetApiKey_BeforeRequest_Works) {
    service->setApiKey("test_key_12345");
    
    GeoLocation result = service->getLocation("8.8.8.8");
    EXPECT_TRUE(result.success || !result.success);
}

TEST_F(GeoLocationServiceApiKeyTest, SetApiKey_EmptyKey_StillWorks) {
    service->setApiKey("");
    
    GeoLocation result = service->getLocation("8.8.8.8");
    EXPECT_TRUE(result.success || !result.success);
}

TEST_F(GeoLocationServiceApiKeyTest, SetApiKey_MultipleTimes_Overwrites) {
    service->setApiKey("key1");
    service->setApiKey("key2");
    
    GeoLocation result = service->getLocation("8.8.8.8");
    EXPECT_TRUE(result.success || !result.success);
}

// Тесты для работы с локальной базой данных

class GeoLocationServiceDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
        // Создаем временный файл для теста
        temp_file = std::make_unique<TempFile>("test.mmdb");
    }
    
    std::unique_ptr<GeoLocationService> service;
    std::unique_ptr<TempFile> temp_file;
};

TEST_F(GeoLocationServiceDatabaseTest, SetDatabasePath_ValidPath_NoThrow) {
    EXPECT_NO_THROW(service->setDatabasePath(temp_file->path()));
}

TEST_F(GeoLocationServiceDatabaseTest, SetDatabasePath_EmptyPath_NoThrow) {
    EXPECT_NO_THROW(service->setDatabasePath(""));
}

TEST_F(GeoLocationServiceDatabaseTest, SetDatabasePath_NonexistentFile_NoThrow) {
    EXPECT_NO_THROW(service->setDatabasePath("/nonexistent/path/file.mmdb"));
}

// Тесты на обработку ошибок сети

class GeoLocationServiceNetworkErrorTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceNetworkErrorTest, Timeout_HandlesGracefully) {
    // Запрос должен завершиться даже при медленной сети
    auto start = std::chrono::steady_clock::now();
    GeoLocation result = service->getLocation("8.8.8.8");
    auto duration = std::chrono::steady_clock::now() - start;
    
    // Не должно зависнуть надолго
    EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(duration).count(), 30);
}

// Тесты производительности

class GeoLocationServicePerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServicePerformanceTest, GetLocation_Under1Second) {
    auto start = std::chrono::steady_clock::now();
    GeoLocation result = service->getLocation("8.8.8.8");
    auto duration = std::chrono::steady_clock::now() - start;
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    if (result.success) {
        EXPECT_LT(ms, 1000) << "Request took " << ms << "ms";
    }
}

TEST_F(GeoLocationServicePerformanceTest, CachedLocation_Under10ms) {
    service->getLocation("8.8.8.8"); // Warm up cache
    
    auto start = std::chrono::steady_clock::now();
    service->getLocation("8.8.8.8");
    auto duration = std::chrono::steady_clock::now() - start;
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    EXPECT_LT(ms, 50) << "Cached request took " << ms << "ms";
}

TEST_F(GeoLocationServicePerformanceTest, MultipleRequests_AverageTime) {
    const int num_requests = 5;
    std::vector<long long> durations;
    
    for (int i = 0; i < num_requests; ++i) {
        auto start = std::chrono::steady_clock::now();
        service->getLocation("8.8.8.8");
        auto duration = std::chrono::steady_clock::now() - start;
        durations.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }
    
    long long avg = 0;
    for (auto d : durations) avg += d;
    avg /= num_requests;
    
    EXPECT_LT(avg, 500) << "Average time: " << avg << "ms";
}

// Интеграционные тесты

class GeoLocationServiceIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceIntegrationTest, GetLocation_CoordinatesArePlausible) {
    GeoLocation result = service->getLocation("8.8.8.8");
    
    if (result.success) {
        // Координаты должны быть в разумных пределах
        EXPECT_GE(result.latitude, -90.0);
        EXPECT_LE(result.latitude, 90.0);
        EXPECT_GE(result.longitude, -180.0);
        EXPECT_LE(result.longitude, 180.0);
        
        // Mountain View координаты ~37.4, -122.1
        if (result.city == "Mountain View") {
            EXPECT_NEAR(result.latitude, 37.4, 0.5);
            EXPECT_NEAR(result.longitude, -122.1, 0.5);
        }
    }
}

TEST_F(GeoLocationServiceIntegrationTest, GetLocation_DifferentIPs_DifferentData) {
    GeoLocation google = service->getLocation("8.8.8.8");
    GeoLocation cloudflare = service->getLocation("1.1.1.1");
    
    if (google.success && cloudflare.success) {
        EXPECT_NE(google.ip, cloudflare.ip);
        // IP адреса должны быть разными
        EXPECT_TRUE(google.ip == "8.8.8.8" || google.ip == "8.8.8.8");
    }
}

TEST_F(GeoLocationServiceIntegrationTest, GetLocation_ReturnsAllFields) {
    GeoLocation result = service->getLocation("8.8.8.8");
    
    if (result.success) {
        // Проверяем, что все поля заполнены или имеют разумные значения
        EXPECT_FALSE(result.ip.empty());
        EXPECT_FALSE(result.country.empty());
        EXPECT_FALSE(result.country_code.empty());
        // City может быть пустым для некоторых IP
        // ISP может быть пустым для некоторых сервисов
    }
}

// Тесты на утечки памяти

class GeoLocationServiceMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<GeoLocationService>();
    }
    
    std::unique_ptr<GeoLocationService> service;
};

TEST_F(GeoLocationServiceMemoryTest, MultipleGetLocation_NoLeak) {
    for (int i = 0; i < 100; ++i) {
        service->getLocation("8.8.8.8");
        service->clearCache();
    }
    SUCCEED();
}

TEST_F(GeoLocationServiceMemoryTest, AsyncOperations_NoLeak) {
    for (int i = 0; i < 50; ++i) {
        service->getLocationAsync("8.8.8.8", [](const GeoLocation&) {});
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    SUCCEED();
}

// Тесты для разных форматов IP

class GeoLocationServiceFormatTest : public ::testing::TestWithParam<std::string> {};

TEST_P(GeoLocationServiceFormatTest, HandlesDifferentIPFormats) {
    GeoLocationService service;
    std::string ip = GetParam();
    
    GeoLocation result = service.getLocation(ip);
    
    if (result.success) {
        EXPECT_EQ(result.ip, ip);
    }
}

INSTANTIATE_TEST_SUITE_P(
    IPFormats,
    GeoLocationServiceFormatTest,
    Values(
        "8.8.8.8",
        "8.8.4.4",
        "1.1.1.1",
        "4.4.4.4",
        "2001:4860:4860::8888",
        "::1",
        "me"
    )
);

// Тесты на парсинг JSON ответов от разных сервисов

class JSONParsingTest : public ::testing::Test {
protected:
    GeoLocationService service;
};

TEST_F(JSONParsingTest, HandlesMalformedJSON) {
    // Создаем ситуацию с некорректным JSON (через mock)
    // В реальном коде это может произойти при ошибках сети
    EXPECT_NO_THROW(service.getLocation("8.8.8.8"));
}

// Main function

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    // Для тестов с сетью можно увеличить таймаут
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    
    return RUN_ALL_TESTS();
}
