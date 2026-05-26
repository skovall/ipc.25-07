#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConfigManager.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using namespace testing;

class ConfigManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаём временную директорию для тестов
        test_dir_ = "temp_test_dir";
        std::filesystem::create_directories(test_dir_);
        config_path_ = test_dir_ + "/test_config.json";
        
        // Удаляем если существует
        std::remove(config_path_.c_str());
        config_ = std::make_unique<ConfigManager>(config_path_);
    }
    
    void TearDown() override {
        config_.reset();
        // Удаляем временный файл и директорию
        std::remove(config_path_.c_str());
        std::filesystem::remove_all(test_dir_);
    }
    
    std::string test_dir_;
    std::string config_path_;
    std::unique_ptr<ConfigManager> config_;
};

TEST_F(ConfigManagerTest, Constructor_CreatesInstance) {
    EXPECT_NO_THROW(ConfigManager config("test_config.json"));
}

TEST_F(ConfigManagerTest, GetValue_ReturnsDefaultIfKeyMissing) {
    int value = config_->getValue<int>("nonexistent", 42);
    EXPECT_EQ(value, 42);
}

TEST_F(ConfigManagerTest, SetAndGetValue_WorksCorrectly) {
    config_->setValue<std::string>("test_key", "test_value");
    std::string value = config_->getValue<std::string>("test_key", "");
    EXPECT_EQ(value, "test_value");
}

TEST_F(ConfigManagerTest, GetValue_DifferentTypes) {
    config_->setValue<int>("int_key", 42);
    config_->setValue<std::string>("string_key", "hello");
    config_->setValue<bool>("bool_key", true);
    
    EXPECT_EQ(config_->getValue<int>("int_key", 0), 42);
    EXPECT_EQ(config_->getValue<std::string>("string_key", ""), "hello");
    EXPECT_TRUE(config_->getValue<bool>("bool_key", false));
}

TEST_F(ConfigManagerTest, SaveAndLoad_PersistsData) {
    // Сохраняем данные
    config_->setValue<std::string>("saved_key", "persisted_value");
    bool save_result = config_->saveConfig();
    EXPECT_TRUE(save_result);
    
    // Загружаем в новый объект
    ConfigManager loaded_config(config_path_);
    bool load_result = loaded_config.loadConfig();
    EXPECT_TRUE(load_result);
    
    std::string value = loaded_config.getValue<std::string>("saved_key", "");
    EXPECT_EQ(value, "persisted_value");
}

TEST_F(ConfigManagerTest, ResetToDefaults_RestoresDefaultValues) {
    config_->setValue<int>("timeout_seconds", 999);
    config_->resetToDefaults();
    
    int timeout = config_->getValue<int>("timeout_seconds", 0);
    EXPECT_EQ(timeout, 10);
}
