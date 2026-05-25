#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <nlohmann/json.hpp>
#include "GeoLocator.h"
#include "TerminalDisplay.h"

using namespace testing;
using json = nlohmann::json;

// Мок для HttpRequest

class MockHttpRequest : public HttpRequest {
public:
    MOCK_METHOD(std::string, get, (const std::string& url), (override));
    MOCK_METHOD(std::string, post, (const std::string& url, const std::string& body), (override));
    MOCK_METHOD(void, setTimeout, (int seconds), (override));
};

// Вспомогательные функции для создания тестовых JSON

std::string createSuccessJsonResponse(const std::string& ip = "8.8.8.8",
                                      const std::string& country = "United States",
                                      const std::string& city = "Mountain View") {
    json response;
    response["status"] = "success";
    response["country"] = country;
    response["countryCode"] = "US";
    response["regionName"] = "California";
    response["city"] = city;
    response["zip"] = "94043";
    response["lat"] = 37.4223;
    response["lon"] = -122.0841;
    response["timezone"] = "America/Los_Angeles";
    response["isp"] = "Google LLC";
    response["org"] = "Google";
    response["as"] = "AS15169 Google LLC";
    response["mobile"] = false;
    response["proxy"] = false;
    response["hosting"] = true;
    response["query"] = ip;
    response["message"] = "";
    
    return response.dump();
}

std::string createFailJsonResponse(const std::string& message = "Invalid IP address") {
    json response;
    response["status"] = "fail";
    response["message"] = message;
    
    return response.dump();
}

std::string createInvalidJson() {
    return "This is not a valid JSON string {{{{";
}

// Фикстура для тестирования GeoLocator

class GeoLocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    void TearDown() override {
        geo_locator_.reset();
        mock_http_.reset();
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

// Тесты для buildApiUrl

TEST_F(GeoLocatorTest, BuildApiUrl_WithValidIP_ReturnsCorrectUrl) {
    // Используем рефлексию или делаем метод публичным для тестирования
    // Альтернатива: тестируем через getDataForIp с моком
    std::string ip = "8.8.8.8";
    std::string expected_prefix = "http://ip-api.com/json/" + ip;
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createSuccessJsonResponse()));
    
    geo_locator_->getDataForIp(ip);
    
    // Проверяем, что URL содержит правильный IP
    EXPECT_CALL(*mock_http_, get(::testing::ContainsRegex(ip)))
        .Times(1);
}

TEST_F(GeoLocatorTest, BuildApiUrl_WithLocalhost_ReturnsCorrectUrl) {
    std::string ip = "127.0.0.1";
    
    EXPECT_CALL(*mock_http_, get(::testing::ContainsRegex("127\\.0\\.0\\.1")))
        .WillOnce(Return(createFailJsonResponse()));
    
    geo_locator_->getDataForIp(ip);
}

TEST_F(GeoLocatorTest, BuildApiUrl_WithIPv6_ReturnsCorrectUrl) {
    std::string ip = "2001:4860:4860::8888";
    
    EXPECT_CALL(*mock_http_, get(::testing::ContainsRegex(ip)))
        .WillOnce(Return(createSuccessJsonResponse(ip)));
    
    geo_locator_->getDataForIp(ip);
}

// Тесты для getDataForIp - успешные сценарии

TEST_F(GeoLocatorTest, GetDataForIp_ValidIPv4_ReturnsSuccessData) {
    std::string ip = "8.8.8.8";
    std::string json_response = createSuccessJsonResponse(ip);
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp(ip);
    
    EXPECT_EQ(result.status, "success");
    EXPECT_EQ(result.ip, ip);
    EXPECT_EQ(result.country, "United States");
    EXPECT_EQ(result.city, "Mountain View");
    EXPECT_EQ(result.countryCode, "US");
    EXPECT_EQ(result.regionName, "California");
    EXPECT_EQ(result.zip, "94043");
    EXPECT_DOUBLE_EQ(result.lat, 37.4223);
    EXPECT_DOUBLE_EQ(result.lon, -122.0841);
    EXPECT_EQ(result.timezone, "America/Los_Angeles");
    EXPECT_EQ(result.isp, "Google LLC");
    EXPECT_EQ(result.org, "Google");
    EXPECT_EQ(result.as, "AS15169 Google LLC");
    EXPECT_FALSE(result.mobile);
    EXPECT_FALSE(result.proxy);
    EXPECT_TRUE(result.hosting);
    EXPECT_TRUE(result.message.empty());
}

TEST_F(GeoLocatorTest, GetDataForIp_ValidIPv6_ReturnsSuccessData) {
    std::string ip = "2001:4860:4860::8888";
    std::string json_response = createSuccessJsonResponse(ip, "United States", "Mountain View");
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp(ip);
    
    EXPECT_EQ(result.status, "success");
    EXPECT_EQ(result.ip, ip);
}

TEST_F(GeoLocatorTest, GetDataForIp_QueryReturnsDifferentIP_UsesDetectedIP) {
    std::string requested_ip = "8.8.8.8";
    std::string detected_ip = "8.8.4.4";
    std::string json_response = createSuccessJsonResponse(detected_ip);
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp(requested_ip);
    
    EXPECT_EQ(result.ip, detected_ip);
    EXPECT_EQ(result.status, "success");
}

TEST_F(GeoLocatorTest, GetDataForIp_SuccessfulRequest_UpdatesLastSuccess) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createSuccessJsonResponse()));
    
    geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_TRUE(geo_locator_->isLastRequestSuccessful());
    EXPECT_TRUE(geo_locator_->getLastErrorMessage().empty());
}

// Тесты для getDataForIp - ошибочные сценарии

TEST_F(GeoLocatorTest, GetDataForIp_EmptyResponse_ReturnsFail) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(""));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_EQ(result.message, "Пустой ответ от API");
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_EQ(geo_locator_->getLastErrorMessage(), "Пустой ответ от API");
}

TEST_F(GeoLocatorTest, GetDataForIp_ApiReturnsFailStatus_ReturnsFailData) {
    std::string error_message = "Invalid IP address";
    std::string json_response = createFailJsonResponse(error_message);
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp("invalid_ip");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_EQ(result.message, error_message);
    EXPECT_EQ(result.country, "Ошибка");
    EXPECT_EQ(result.city, "Ошибка");
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_EQ(geo_locator_->getLastErrorMessage(), error_message);
}

TEST_F(GeoLocatorTest, GetDataForIp_HttpThrowsException_ReturnsFail) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Throw(std::runtime_error("Connection timeout")));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_THAT(result.message, HasSubstr("HTTP ошибка"));
    EXPECT_THAT(result.message, HasSubstr("Connection timeout"));
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

TEST_F(GeoLocatorTest, GetDataForIp_InvalidJsonResponse_ReturnsFail) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createInvalidJson()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_THAT(result.message, HasSubstr("Ошибка парсинга JSON"));
    EXPECT_EQ(result.country, "Ошибка");
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

TEST_F(GeoLocatorTest, GetDataForIp_MissingRequiredFields_HandlesGracefully) {
    json incomplete_response;
    incomplete_response["status"] = "success";
    incomplete_response["country"] = "United States";
    // Пропускаем city и другие поля
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(incomplete_response.dump()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.status, "success");
    EXPECT_EQ(result.country, "United States");
    EXPECT_EQ(result.city, "Неизвестно"); // Значение по умолчанию
    EXPECT_EQ(result.zip, "N/A");
}

TEST_F(GeoLocatorTest, GetDataForIp_NetworkError_HandlesGracefully) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Throw(std::runtime_error("Network unreachable")));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_FALSE(result.message.empty());
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

// Тесты для parseApiResponse

class GeoLocatorParseTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorParseTest, ParseApiResponse_SuccessJson_ParsesAllFields) {
    std::string ip = "8.8.8.8";
    std::string json_response = createSuccessJsonResponse(ip);
    
    // Через публичный метод
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp(ip);
    
    EXPECT_EQ(result.ip, ip);
    EXPECT_EQ(result.country, "United States");
    EXPECT_EQ(result.countryCode, "US");
    EXPECT_EQ(result.regionName, "California");
    EXPECT_EQ(result.city, "Mountain View");
    EXPECT_EQ(result.zip, "94043");
    EXPECT_DOUBLE_EQ(result.lat, 37.4223);
    EXPECT_DOUBLE_EQ(result.lon, -122.0841);
    EXPECT_EQ(result.timezone, "America/Los_Angeles");
    EXPECT_EQ(result.isp, "Google LLC");
}

TEST_F(GeoLocatorParseTest, ParseApiResponse_SuccessJson_WithBooleanFields) {
    json response;
    response["status"] = "success";
    response["mobile"] = true;
    response["proxy"] = true;
    response["hosting"] = false;
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(response.dump()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_TRUE(result.mobile);
    EXPECT_TRUE(result.proxy);
    EXPECT_FALSE(result.hosting);
}

TEST_F(GeoLocatorParseTest, ParseApiResponse_FailJson_ReturnsErrorData) {
    std::string error_msg = "Private IP address";
    std::string json_response = createFailJsonResponse(error_msg);
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    GeoLocationData result = geo_locator_->getDataForIp("192.168.1.1");
    
    EXPECT_EQ(result.status, "fail");
    EXPECT_EQ(result.message, error_msg);
    EXPECT_EQ(result.country, "Ошибка");
    EXPECT_EQ(result.city, "Ошибка");
}

// Тесты для getMyPublicIp

TEST_F(GeoLocatorTest, GetMyPublicIp_Successful_ReturnsIP) {
    std::string expected_ip = "8.8.8.8";
    
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return(expected_ip + "\n"));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_EQ(result, expected_ip);
    EXPECT_TRUE(geo_locator_->isLastRequestSuccessful());
    EXPECT_TRUE(geo_locator_->getLastErrorMessage().empty());
}

TEST_F(GeoLocatorTest, GetMyPublicIp_WithoutNewline_ReturnsIP) {
    std::string expected_ip = "1.2.3.4";
    
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return(expected_ip));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_EQ(result, expected_ip);
    EXPECT_TRUE(geo_locator_->isLastRequestSuccessful());
}

TEST_F(GeoLocatorTest, GetMyPublicIp_WithMultipleNewlines_StripsAll) {
    std::string expected_ip = "8.8.8.8";
    
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return(expected_ip + "\n\r\n"));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_EQ(result, expected_ip);
}

TEST_F(GeoLocatorTest, GetMyPublicIp_EmptyResponse_ReturnsEmptyAndSetsError) {
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return(""));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_EQ(geo_locator_->getLastErrorMessage(), "Не удалось определить IP (пустой ответ)");
}

TEST_F(GeoLocatorTest, GetMyPublicIp_WhitespaceOnly_ReturnsEmptyAndSetsError) {
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return("   \n\t\r"));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

TEST_F(GeoLocatorTest, GetMyPublicIp_HttpException_ReturnsEmptyAndSetsError) {
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Throw(std::runtime_error("DNS lookup failed")));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_THAT(geo_locator_->getLastErrorMessage(), HasSubstr("DNS lookup failed"));
}

TEST_F(GeoLocatorTest, GetMyPublicIp_InvalidIPResponse_StillReturnsAsIs) {
    std::string invalid_response = "not_an_ip";
    
    EXPECT_CALL(*mock_http_, get("https://api.ipify.org"))
        .WillOnce(Return(invalid_response));
    
    std::string result = geo_locator_->getMyPublicIp();
    
    // Сервис возвращает что угодно, мы не валидируем
    EXPECT_EQ(result, invalid_response);
}

// Тесты для isLastRequestSuccessful и getLastErrorMessage

TEST_F(GeoLocatorTest, LastRequestStatus_InitiallyFalse) {
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_TRUE(geo_locator_->getLastErrorMessage().empty());
}

TEST_F(GeoLocatorTest, LastRequestStatus_AfterSuccess_IsTrue) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createSuccessJsonResponse()));
    
    geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_TRUE(geo_locator_->isLastRequestSuccessful());
    EXPECT_TRUE(geo_locator_->getLastErrorMessage().empty());
}

TEST_F(GeoLocatorTest, LastRequestStatus_AfterFail_IsFalseWithError) {
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(""));
    
    geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
    EXPECT_FALSE(geo_locator_->getLastErrorMessage().empty());
}

TEST_F(GeoLocatorTest, LastRequestStatus_AfterMultipleRequests_TracksLast) {
    // Первый запрос - успешный
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createSuccessJsonResponse()))
        .WillOnce(Return(""));
    
    geo_locator_->getDataForIp("8.8.8.8");
    EXPECT_TRUE(geo_locator_->isLastRequestSuccessful());
    
    // Второй запрос - неудачный
    geo_locator_->getDataForIp("invalid");
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

// Интеграционные тесты с разными типами IP

class GeoLocatorIntegrationTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_P(GeoLocatorIntegrationTest, VariousIPs_HandlesCorrectly) {
    auto [ip, should_succeed] = GetParam();
    
    if (should_succeed) {
        EXPECT_CALL(*mock_http_, get(::testing::_))
            .WillOnce(Return(createSuccessJsonResponse(ip)));
    } else {
        EXPECT_CALL(*mock_http_, get(::testing::_))
            .WillOnce(Return(createFailJsonResponse("Invalid IP")));
    }
    
    GeoLocationData result = geo_locator_->getDataForIp(ip);
    
    if (should_succeed) {
        EXPECT_EQ(result.status, "success");
    } else {
        EXPECT_EQ(result.status, "fail");
    }
}

INSTANTIATE_TEST_SUITE_P(
    IPVariants,
    GeoLocatorIntegrationTest,
    Values(
        std::make_tuple("8.8.8.8", true),
        std::make_tuple("1.1.1.1", true),
        std::make_tuple("4.4.4.4", true),
        std::make_tuple("2001:4860:4860::8888", true),
        std::make_tuple("invalid", false),
        std::make_tuple("256.1.1.1", false),
        std::make_tuple("192.168.1.1", false),  // private IP
        std::make_tuple("", false)
    )
);

// Тесты для работы с различными полями JSON

class GeoLocatorFieldTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorFieldTest, MissingOptionalFields_UsesDefaults) {
    json minimal_response;
    minimal_response["status"] = "success";
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(minimal_response.dump()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_EQ(result.country, "Неизвестно");
    EXPECT_EQ(result.city, "Неизвестно");
    EXPECT_EQ(result.zip, "N/A");
    EXPECT_EQ(result.timezone, "N/A");
    EXPECT_DOUBLE_EQ(result.lat, 0.0);
    EXPECT_DOUBLE_EQ(result.lon, 0.0);
}

TEST_F(GeoLocatorFieldTest, NumericFields_AreParsedCorrectly) {
    json response;
    response["status"] = "success";
    response["lat"] = 55.7558;
    response["lon"] = 37.6176;
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(response.dump()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_DOUBLE_EQ(result.lat, 55.7558);
    EXPECT_DOUBLE_EQ(result.lon, 37.6176);
}

TEST_F(GeoLocatorFieldTest, BooleanFields_DefaultToFalse) {
    json response;
    response["status"] = "success";
    // Не включаем mobile, proxy, hosting
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(response.dump()));
    
    GeoLocationData result = geo_locator_->getDataForIp("8.8.8.8");
    
    EXPECT_FALSE(result.mobile);
    EXPECT_FALSE(result.proxy);
    EXPECT_FALSE(result.hosting);
}

// Тесты производительности

class GeoLocatorPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorPerformanceTest, GetDataForIp_Under100ms) {
    std::string json_response = createSuccessJsonResponse();
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(json_response));
    
    auto start = std::chrono::high_resolution_clock::now();
    geo_locator_->getDataForIp("8.8.8.8");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100);
}

TEST_F(GeoLocatorPerformanceTest, ParseApiResponse_Under10ms) {
    std::string json_response = createSuccessJsonResponse();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        EXPECT_CALL(*mock_http_, get(::testing::_))
            .WillOnce(Return(json_response));
        geo_locator_->getDataForIp("8.8.8.8");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000); // 1000 запросов за <1 секунды
}

// Тесты на обработку специальных символов

class GeoLocatorSpecialCharsTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorSpecialCharsTest, IPWithSpaces_IsHandled) {
    std::string ip_with_space = " 8.8.8.8 ";
    
    EXPECT_CALL(*mock_http_, get(::testing::ContainsRegex("8\\.8\\.8\\.8")))
        .WillOnce(Return(createSuccessJsonResponse()));
    
    EXPECT_NO_THROW(geo_locator_->getDataForIp(ip_with_space));
}

TEST_F(GeoLocatorSpecialCharsTest, VeryLongIP_IsHandled) {
    std::string long_ip(10000, '1');
    
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(Return(createFailJsonResponse("Invalid IP")));
    
    EXPECT_NO_THROW(geo_locator_->getDataForIp(long_ip));
}

// Тесты на корректность URL построения

class GeoLocatorUrlTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_http_ = std::make_shared<MockHttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*mock_http_);
    }
    
    std::shared_ptr<MockHttpRequest> mock_http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorUrlTest, BuildApiUrl_ContainsAllFields) {
    std::string expected_fields[] = {
        "fields=status,message,country,countryCode,regionName,city",
        "zip,lat,lon,timezone,isp,org,as,mobile,proxy,hosting,query"
    };
    
    std::string full_url;
    EXPECT_CALL(*mock_http_, get(::testing::_))
        .WillOnce(::testing::Invoke([&full_url](const std::string& url) {
            full_url = url;
            return createSuccessJsonResponse();
        }));
    
    geo_locator_->getDataForIp("8.8.8.8");
    
    for (const auto& field : expected_fields) {
        EXPECT_THAT(full_url, HasSubstr(field));
    }
}

// Main function

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    return RUN_ALL_TESTS();
}
