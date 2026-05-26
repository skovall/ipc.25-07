#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "History.h"

using namespace testing;

class HistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаём временную директорию
        test_dir_ = "temp_history_dir";
        std::filesystem::create_directories(test_dir_);
        test_file_ = test_dir_ + "/history.json";
        
        // Удаляем если существует
        std::remove(test_file_.c_str());
        history_ = std::make_unique<History>(test_file_);
    }
    
    void TearDown() override {
        history_.reset();
        // Удаляем временный файл и директорию
        std::remove(test_file_.c_str());
        std::filesystem::remove_all(test_dir_);
    }
    
    GeoLocationData createTestData(const std::string& ip = "8.8.8.8") {
        GeoLocationData data;
        data.ip = ip;
        data.country = "United States";
        data.countryCode = "US";
        data.city = "Mountain View";
        data.status = "success";
        return data;
    }
    
    std::string test_dir_;
    std::string test_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryTest, AddEntry_AddsOneEntry) {
    auto data = createTestData();
    history_->addEntry(data);
    
    ASSERT_EQ(history_->entries().size(), 1);
    EXPECT_EQ(history_->entries()[0].ip, "8.8.8.8");
}

TEST_F(HistoryTest, AddEntry_AddsMultipleEntries) {
    history_->addEntry(createTestData("8.8.8.8"));
    history_->addEntry(createTestData("1.1.1.1"));
    
    ASSERT_EQ(history_->entries().size(), 2);
    EXPECT_EQ(history_->entries()[0].ip, "8.8.8.8");
    EXPECT_EQ(history_->entries()[1].ip, "1.1.1.1");
}

TEST_F(HistoryTest, Clear_RemovesAllEntries) {
    history_->addEntry(createTestData("8.8.8.8"));
    history_->clear();
    
    EXPECT_TRUE(history_->entries().empty());
}

TEST_F(HistoryTest, SaveAndLoad_PersistsData) {
    // Добавляем запись
    auto data = createTestData("8.8.8.8");
    history_->addEntry(data);
    
    // Сохраняем
    bool save_result = history_->save();
    EXPECT_TRUE(save_result);
    
    // Создаём новый объект и загружаем
    History loaded_history(test_file_);
    bool load_result = loaded_history.load();
    EXPECT_TRUE(load_result);
    
    // Проверяем
    ASSERT_EQ(loaded_history.entries().size(), 1);
    EXPECT_EQ(loaded_history.entries()[0].ip, "8.8.8.8");
}

TEST_F(HistoryTest, SetMaxEntries_RespectsLimit) {
    history_->setMaxEntries(2);
    history_->addEntry(createTestData("1.1.1.1"));
    history_->addEntry(createTestData("2.2.2.2"));
    history_->addEntry(createTestData("3.3.3.3"));
    
    EXPECT_EQ(history_->entries().size(), 2);
}

TEST_F(HistoryTest, LoadFromNonexistentFile_ReturnsFalse) {
    History bad_history("nonexistent_file_12345.json");
    EXPECT_FALSE(bad_history.load());
}
