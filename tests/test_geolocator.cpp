#include <gtest/gtest.h>
#include "GeoLocator.h"
#include "HttpRequest.h"
#include "nlohmann/json.hpp"

class GeoLocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        http_ = std::make_unique<HttpRequest>();
        geo_locator_ = std::make_unique<GeoLocator>(*http_);
    }
    
    void TearDown() override {
        geo_locator_.reset();
        http_.reset();
    }
    
    std::unique_ptr<HttpRequest> http_;
    std::unique_ptr<GeoLocator> geo_locator_;
};

TEST_F(GeoLocatorTest, Constructor_CreatesInstance) {
    HttpRequest req;
    EXPECT_NO_THROW(GeoLocator loc(req));
}

TEST_F(GeoLocatorTest, IsLastRequestSuccessful_InitiallyFalse) {
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}

TEST_F(GeoLocatorTest, GetDataForIp_InvalidIP_ReturnsFail) {
    GeoLocationData result = geo_locator_->getDataForIp("invalid_ip_address");
    EXPECT_EQ(result.status, "fail");
    EXPECT_FALSE(result.message.empty());
}

TEST_F(GeoLocatorTest, GetDataForIp_EmptyIP_ReturnsFailOrSuccess) {
    // Пустой IP может обрабатываться по-разному в зависимости от реализации
    GeoLocationData result = geo_locator_->getDataForIp("");
    // Проверяем, что результат не пустой (либо success, либо fail)
    EXPECT_FALSE(result.status.empty());
    // Если статус success, проверяем что IP не пустой
    if (result.status == "success") {
        EXPECT_FALSE(result.ip.empty());
    }
}

TEST_F(GeoLocatorTest, GetLastErrorMessage_InitiallyEmpty) {
    EXPECT_TRUE(geo_locator_->getLastErrorMessage().empty());
}

TEST_F(GeoLocatorTest, AfterInvalidIP_ErrorMessageNotEmpty) {
    geo_locator_->getDataForIp("invalid_ip");
    EXPECT_FALSE(geo_locator_->getLastErrorMessage().empty());
    EXPECT_FALSE(geo_locator_->isLastRequestSuccessful());
}
