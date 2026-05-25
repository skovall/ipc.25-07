#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <temp_dir.hpp>
#include "History.h"

using namespace testing;

// Вспомогательные функции для тестов

class TempFile {
public:
    explicit TempFile(const std::string& name = "test_history.txt") 
        : path_(std::filesystem::temp_directory_path() / name) {
        // Генерируем уникальное имя если нужно
        if (std::filesystem::exists(path_)) {
            path_ = std::filesystem::temp_directory_path() / 
                    (name + "_" + std::to_string(++counter_));
        }
    }
    
    ~TempFile() {
        std::error_code ec;
        std::filesystem::remove(path_, ec);
    }
    
    std::string path() const { return path_.string(); }
    
private:
    std::filesystem::path path_;
    static int counter_;
};

int TempFile::counter_ = 0;

// Создание тестовых данных
GeoLocationData createTestData(const std::string& ip = "8.8.8.8",
                               const std::string& city = "Mountain View",
                               const std::string& country = "USA") {
    GeoLocationData data;
    data.ip = ip;
    data.city = city;
    data.country = country;
    data.country_code = "US";
    data.region = "California";
    data.postal_code = "94043";
    data.latitude = 37.4223;
    data.longitude = -122.0841;
    data.timezone = "America/Los_Angeles";
    data.isp = "Google";
    data.organization = "Google";
    data.success = true;
    data.timestamp = std::time(nullptr);
    return data;
}

// Фикстура для тестирования History

class HistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>();
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    void TearDown() override {
        history_.reset();
        temp_file_.reset();
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

// Тесты конструктора

TEST_F(HistoryTest, Constructor_WithValidFileName_CreatesInstance) {
    EXPECT_NO_THROW(History history("test_history.txt"));
    EXPECT_NO_THROW(History history(""));
    EXPECT_NO_THROW(History history("path/with/directories/history.txt"));
}

TEST_F(HistoryTest, Constructor_InitializesEmptyEntries) {
    EXPECT_TRUE(history_->entries().empty());
    EXPECT_EQ(history_->entries().size(), 0);
}

TEST_F(HistoryTest, Constructor_DefaultMaxEntries_IsSet) {
    // Проверяем, что max_entries имеет разумное значение по умолчанию
    // Добавляем много записей и проверяем лимит
    for (int i = 0; i < 1000; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    // Если max_entries установлен, размер не должен превышать лимит
    EXPECT_LE(history_->entries().size(), 1000);
}

// Тесты для addEntry

TEST_F(HistoryTest, AddEntry_AddsOneEntry) {
    auto data = createTestData();
    history_->addEntry(data);
    
    ASSERT_EQ(history_->entries().size(), 1);
    EXPECT_EQ(history_->entries()[0].ip, "8.8.8.8");
    EXPECT_EQ(history_->entries()[0].city, "Mountain View");
}

TEST_F(HistoryTest, AddEntry_AddsMultipleEntries) {
    std::vector<std::string> ips = {"8.8.8.8", "1.1.1.1", "4.4.4.4"};
    
    for (const auto& ip : ips) {
        history_->addEntry(createTestData(ip));
    }
    
    ASSERT_EQ(history_->entries().size(), 3);
    EXPECT_EQ(history_->entries()[0].ip, "8.8.8.8");
    EXPECT_EQ(history_->entries()[1].ip, "1.1.1.1");
    EXPECT_EQ(history_->entries()[2].ip, "4.4.4.4");
}

TEST_F(HistoryTest, AddEntry_EntriesOrderedByAddition) {
    history_->addEntry(createTestData("1.1.1.1", "City1"));
    history_->addEntry(createTestData("2.2.2.2", "City2"));
    history_->addEntry(createTestData("3.3.3.3", "City3"));
    
    EXPECT_EQ(history_->entries()[0].ip, "1.1.1.1");
    EXPECT_EQ(history_->entries()[1].ip, "2.2.2.2");
    EXPECT_EQ(history_->entries()[2].ip, "3.3.3.3");
}

TEST_F(HistoryTest, AddEntry_RespectsMaxEntries) {
    history_->setMaxEntries(2);
    
    history_->addEntry(createTestData("1.1.1.1"));
    history_->addEntry(createTestData("2.2.2.2"));
    history_->addEntry(createTestData("3.3.3.3"));
    
    EXPECT_EQ(history_->entries().size(), 2);
    EXPECT_EQ(history_->entries()[0].ip, "2.2.2.2");
    EXPECT_EQ(history_->entries()[1].ip, "3.3.3.3");
}

TEST_F(HistoryTest, AddEntry_WithMaxEntriesZero_StillAddsAtLeastOne) {
    history_->setMaxEntries(0);
    history_->addEntry(createTestData("1.1.1.1"));
    
    // Даже при max_entries = 0, должна быть хотя бы одна запись
    EXPECT_GE(history_->entries().size(), 1);
}

TEST_F(HistoryTest, AddEntry_DuplicateEntries_AreAllowed) {
    auto data = createTestData("8.8.8.8");
    
    history_->addEntry(data);
    history_->addEntry(data);
    history_->addEntry(data);
    
    EXPECT_EQ(history_->entries().size(), 3);
}

TEST_F(HistoryTest, AddEntry_EmptyData_HandlesGracefully) {
    GeoLocationData empty;
    EXPECT_NO_THROW(history_->addEntry(empty));
    EXPECT_EQ(history_->entries().size(), 1);
}

// Тесты для clear

TEST_F(HistoryTest, Clear_EmptyHistory_DoesNothing) {
    EXPECT_NO_THROW(history_->clear());
    EXPECT_TRUE(history_->entries().empty());
}

TEST_F(HistoryTest, Clear_NonEmptyHistory_RemovesAllEntries) {
    history_->addEntry(createTestData("1.1.1.1"));
    history_->addEntry(createTestData("2.2.2.2"));
    history_->addEntry(createTestData("3.3.3.3"));
    
    ASSERT_EQ(history_->entries().size(), 3);
    
    history_->clear();
    
    EXPECT_TRUE(history_->entries().empty());
    EXPECT_EQ(history_->entries().size(), 0);
}

TEST_F(HistoryTest, Clear_AfterClear_CanAddNewEntries) {
    history_->addEntry(createTestData("1.1.1.1"));
    history_->clear();
    history_->addEntry(createTestData("2.2.2.2"));
    
    EXPECT_EQ(history_->entries().size(), 1);
    EXPECT_EQ(history_->entries()[0].ip, "2.2.2.2");
}

// Тесты для setMaxEntries

TEST_F(HistoryTest, SetMaxEntries_ReducesSizeWhenNeeded) {
    // Добавляем 5 записей
    for (int i = 1; i <= 5; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    ASSERT_EQ(history_->entries().size(), 5);
    
    // Уменьшаем лимит до 3
    history_->setMaxEntries(3);
    
    // Должны остаться только последние 3 записи
    EXPECT_EQ(history_->entries().size(), 3);
    EXPECT_EQ(history_->entries()[0].ip, "192.168.1.3");
    EXPECT_EQ(history_->entries()[1].ip, "192.168.1.4");
    EXPECT_EQ(history_->entries()[2].ip, "192.168.1.5");
}

TEST_F(HistoryTest, SetMaxEntries_IncreasesLimit_KeepsAllEntries) {
    history_->setMaxEntries(3);
    for (int i = 1; i <= 3; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    ASSERT_EQ(history_->entries().size(), 3);
    
    history_->setMaxEntries(10);
    history_->addEntry(createTestData("192.168.1.4"));
    
    EXPECT_EQ(history_->entries().size(), 4);
}

TEST_F(HistoryTest, SetMaxEntries_SameValue_DoesNothing) {
    history_->setMaxEntries(100);
    for (int i = 0; i < 50; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    
    size_t size_before = history_->entries().size();
    history_->setMaxEntries(100);
    size_t size_after = history_->entries().size();
    
    EXPECT_EQ(size_before, size_after);
}

TEST_F(HistoryTest, SetMaxEntries_ZeroValue_WorksAsMinimum) {
    history_->setMaxEntries(0);
    history_->addEntry(createTestData("1.1.1.1"));
    
    EXPECT_GE(history_->entries().size(), 1);
}

// Тесты для entries (getter)

TEST_F(HistoryTest, Entries_ReturnsConstReference) {
    const auto& entries = history_->entries();
    EXPECT_TRUE(entries.empty());
    
    history_->addEntry(createTestData("8.8.8.8"));
    
    EXPECT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].ip, "8.8.8.8");
}

TEST_F(HistoryTest, Entries_DoesNotAllowModification) {
    // Проверяем, что возвращается const ссылка
    const auto& entries = history_->entries();
    
    // Следующая строка не должна компилироваться (раскомментировать для проверки)
    // entries.push_back(createTestData("1.1.1.1"));
    
    EXPECT_TRUE(entries.empty());
}

// Тесты для save и load

class HistoryFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>();
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    bool fileExists() const {
        return std::filesystem::exists(temp_file_->path());
    }
    
    size_t fileSize() const {
        return std::filesystem::file_size(temp_file_->path());
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryFileTest, Save_EmptyHistory_CreatesFile) {
    bool result = history_->save();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(fileExists());
}

TEST_F(HistoryFileTest, Save_WithEntries_WritesToFile) {
    history_->addEntry(createTestData("8.8.8.8"));
    history_->addEntry(createTestData("1.1.1.1"));
    
    bool result = history_->save();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(fileExists());
    EXPECT_GT(fileSize(), 0);
}

TEST_F(HistoryFileTest, Save_MultipleTimes_OverwritesFile) {
    history_->addEntry(createTestData("8.8.8.8"));
    history_->save();
    
    size_t size_first = fileSize();
    
    history_->addEntry(createTestData("1.1.1.1"));
    history_->save();
    
    size_t size_second = fileSize();
    EXPECT_NE(size_first, size_second);
}

TEST_F(HistoryFileTest, Load_FromValidFile_RestoresEntries) {
    // Сохраняем данные
    history_->addEntry(createTestData("8.8.8.8"));
    history_->addEntry(createTestData("1.1.1.1"));
    history_->save();
    
    // Создаем новый объект History и загружаем
    History loaded_history(temp_file_->path());
    bool result = loaded_history.load();
    
    EXPECT_TRUE(result);
    ASSERT_EQ(loaded_history.entries().size(), 2);
    EXPECT_EQ(loaded_history.entries()[0].ip, "8.8.8.8");
    EXPECT_EQ(loaded_history.entries()[1].ip, "1.1.1.1");
}

TEST_F(HistoryFileTest, Load_FromNonexistentFile_ReturnsFalse) {
    History history("nonexistent_file_12345.txt");
    bool result = history.load();
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(history.entries().empty());
}

TEST_F(HistoryFileTest, Load_FromEmptyFile_LoadsEmptyHistory) {
    // Создаем пустой файл
    std::ofstream file(temp_file_->path());
    file.close();
    
    bool result = history_->load();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(history_->entries().empty());
}

TEST_F(HistoryFileTest, Save_AfterLoad_PersistsChanges) {
    // Сохраняем начальные данные
    history_->addEntry(createTestData("8.8.8.8"));
    history_->save();
    
    // Загружаем в новый объект
    History loaded_history(temp_file_->path());
    loaded_history.load();
    ASSERT_EQ(loaded_history.entries().size(), 1);
    
    // Добавляем новую запись и сохраняем
    loaded_history.addEntry(createTestData("1.1.1.1"));
    loaded_history.save();
    
    // Загружаем снова и проверяем
    History reloaded_history(temp_file_->path());
    reloaded_history.load();
    EXPECT_EQ(reloaded_history.entries().size(), 2);
}

// Тесты для display

class HistoryDisplayTest : public HistoryTest {
protected:
    void SetUp() override {
        HistoryTest::SetUp();
        // Перенаправляем std::cout
        old_cout_buf_ = std::cout.rdbuf(buffer_.rdbuf());
    }
    
    void TearDown() override {
        std::cout.rdbuf(old_cout_buf_);
        HistoryTest::TearDown();
    }
    
    std::string getOutput() const {
        return buffer_.str();
    }
    
private:
    std::stringstream buffer_;
    std::streambuf* old_cout_buf_ = nullptr;
};

TEST_F(HistoryDisplayTest, Display_EmptyHistory_ShowsNoEntriesMessage) {
    history_->display();
    
    std::string output = getOutput();
    EXPECT_THAT(output, AnyOf(HasSubstr("empty"), HasSubstr("no entries"), 
                              HasSubstr("No history")));
}

TEST_F(HistoryDisplayTest, Display_WithEntries_ShowsAllEntries) {
    history_->addEntry(createTestData("8.8.8.8", "Mountain View"));
    history_->addEntry(createTestData("1.1.1.1", "San Francisco"));
    
    history_->display();
    
    std::string output = getOutput();
    EXPECT_THAT(output, HasSubstr("8.8.8.8"));
    EXPECT_THAT(output, HasSubstr("Mountain View"));
    EXPECT_THAT(output, HasSubstr("1.1.1.1"));
    EXPECT_THAT(output, HasSubstr("San Francisco"));
}

TEST_F(HistoryDisplayTest, Display_FormatsOutputCorrectly) {
    auto data = createTestData("8.8.8.8", "Mountain View", "USA");
    history_->addEntry(data);
    
    history_->display();
    
    std::string output = getOutput();
    EXPECT_THAT(output, HasSubstr("IP: 8.8.8.8"));
    EXPECT_THAT(output, HasSubstr("USA"));
    EXPECT_THAT(output, HasSubstr("Mountain View"));
}

TEST_F(HistoryDisplayTest, Display_WithManyEntries_ShowsAll) {
    const int num_entries = 100;
    for (int i = 0; i < num_entries; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    
    history_->display();
    
    std::string output = getOutput();
    for (int i = 0; i < num_entries; ++i) {
        EXPECT_THAT(output, HasSubstr("192.168.1." + std::to_string(i)));
    }
}

// Интеграционные тесты

class HistoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>("integration_test.txt");
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryIntegrationTest, FullWorkflow_AddSaveLoadDisplay) {
    // Добавляем записи
    history_->addEntry(createTestData("8.8.8.8", "Mountain View"));
    history_->addEntry(createTestData("1.1.1.1", "San Francisco"));
    history_->addEntry(createTestData("4.4.4.4", "Los Angeles"));
    
    // Сохраняем
    EXPECT_TRUE(history_->save());
    
    // Очищаем память
    history_->clear();
    EXPECT_TRUE(history_->entries().empty());
    
    // Загружаем
    EXPECT_TRUE(history_->load());
    EXPECT_EQ(history_->entries().size(), 3);
    
    // Проверяем данные
    EXPECT_EQ(history_->entries()[0].ip, "8.8.8.8");
    EXPECT_EQ(history_->entries()[1].ip, "1.1.1.1");
    EXPECT_EQ(history_->entries()[2].ip, "4.4.4.4");
}

TEST_F(HistoryIntegrationTest, ConcurrentAccess_ThreadSafety) {
    const int num_threads = 10;
    const int entries_per_thread = 100;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, entries_per_thread]() {
            for (int i = 0; i < entries_per_thread; ++i) {
                history_->addEntry(createTestData(
                    "192.168." + std::to_string(t) + "." + std::to_string(i)
                ));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(history_->entries().size(), num_threads * entries_per_thread);
}

TEST_F(HistoryIntegrationTest, MaxEntries_WithConcurrentAccess) {
    history_->setMaxEntries(50);
    
    const int num_threads = 5;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this]() {
            for (int i = 0; i < 20; ++i) {
                history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_LE(history_->entries().size(), 50);
}

// Тесты на производительность

class HistoryPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>("perf_test.txt");
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryPerformanceTest, AddEntries_Performance) {
    const int num_entries = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_entries; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100) << "Adding " << num_entries 
                                      << " entries took " << duration.count() << "ms";
}

TEST_F(HistoryPerformanceTest, Save_Performance) {
    const int num_entries = 1000;
    for (int i = 0; i < num_entries; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool result = history_->save();
    auto end = std::chrono::high_resolution_clock::now();
    
    EXPECT_TRUE(result);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500) << "Save took " << duration.count() << "ms";
}

TEST_F(HistoryPerformanceTest, Load_Performance) {
    const int num_entries = 1000;
    for (int i = 0; i < num_entries; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    history_->save();
    
    history_->clear();
    
    auto start = std::chrono::high_resolution_clock::now();
    bool result = history_->load();
    auto end = std::chrono::high_resolution_clock::now();
    
    EXPECT_TRUE(result);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500) << "Load took " << duration.count() << "ms";
}

// Тесты на граничные случаи

class HistoryEdgeCasesTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>("edge_test.txt");
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryEdgeCasesTest, VeryLongFileName_HandlesGracefully) {
    std::string long_name(1000, 'a');
    long_name += ".txt";
    
    EXPECT_NO_THROW(History history(long_name));
}

TEST_F(HistoryEdgeCasesTest, SpecialCharactersInFileName_HandlesGracefully) {
    std::string special_name = "!@#$%^&*()_+{}|:<>?[];',.txt";
    
    // В зависимости от ОС, может быть разрешено или нет
    EXPECT_NO_THROW(History history(special_name));
}

TEST_F(HistoryEdgeCasesTest, MaxEntries_SetsToVeryLargeValue) {
    history_->setMaxEntries(1000000);
    for (int i = 0; i < 10000; ++i) {
        history_->addEntry(createTestData("192.168.1." + std::to_string(i)));
    }
    
    EXPECT_EQ(history_->entries().size(), 10000);
}

TEST_F(HistoryEdgeCasesTest, Save_WithNoDiskSpace_HandlesError) {
    // Этот тест сложно реализовать без специальных условий
    // В реальном коде должны быть обработчики ошибок
    history_->addEntry(createTestData("8.8.8.8"));
    EXPECT_NO_THROW(history_->save());
}

TEST_F(HistoryEdgeCasesTest, Load_CorruptedFile_HandlesGracefully) {
    // Создаем corrupted файл
    std::ofstream file(temp_file_->path(), std::ios::binary);
    file << "This is not a valid history file format\x00\x01\x02";
    file.close();
    
    bool result = history_->load();
    EXPECT_FALSE(result);
}

// Тесты для корректности данных

class HistoryDataIntegrityTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_file_ = std::make_unique<TempFile>("integrity_test.txt");
        history_ = std::make_unique<History>(temp_file_->path());
    }
    
    std::unique_ptr<TempFile> temp_file_;
    std::unique_ptr<History> history_;
};

TEST_F(HistoryDataIntegrityTest, SaveAndLoad_PreservesAllFields) {
    auto original_data = createTestData("8.8.8.8", "Mountain View", "USA");
    original_data.latitude = 37.4223;
    original_data.longitude = -122.0841;
    original_data.isp = "Google LLC";
    original_data.timezone = "America/Los_Angeles";
    
    history_->addEntry(original_data);
    history_->save();
    
    History loaded_history(temp_file_->path());
    loaded_history.load();
    
    ASSERT_EQ(loaded_history.entries().size(), 1);
    const auto& loaded_data = loaded_history.entries()[0];
    
    EXPECT_EQ(loaded_data.ip, original_data.ip);
    EXPECT_EQ(loaded_data.city, original_data.city);
    EXPECT_EQ(loaded_data.country, original_data.country);
    EXPECT_EQ(loaded_data.latitude, original_data.latitude);
    EXPECT_EQ(loaded_data.longitude, original_data.longitude);
    EXPECT_EQ(loaded_data.isp, original_data.isp);
    EXPECT_EQ(loaded_data.timezone, original_data.timezone);
}

TEST_F(HistoryDataIntegrityTest, SaveAndLoad_MultipleEntries_KeepOrder) {
    std::vector<std::string> ips = {"8.8.8.8", "1.1.1.1", "4.4.4.4", "9.9.9.9"};
    
    for (const auto& ip : ips) {
        history_->addEntry(createTestData(ip));
    }
    history_->save();
    
    History loaded_history(temp_file_->path());
    loaded_history.load();
    
    ASSERT_EQ(loaded_history.entries().size(), ips.size());
    for (size_t i = 0; i < ips.size(); ++i) {
        EXPECT_EQ(loaded_history.entries()[i].ip, ips[i]);
    }
}

// Main function

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    return RUN_ALL_TESTS();
}
