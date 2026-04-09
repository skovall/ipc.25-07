#include "НастяHttp.h"
#include <iostream>
#include <stdexcept>
#include <mutex>

// Глобальная инициализация libcurl (один раз для всей программы)
static std::once_flag curl_global_init_flag;

HttpRequest::HttpRequest()
    : curl_handle_(nullptr)
    , verbose_mode_(false)
    , timeout_seconds_(30)
    , user_agent_("Mozilla/5.0 (compatible; HttpRequest/1.0)")
    , last_http_code_(0)
{
    // Инициализируем глобальное состояние libcurl (безопасно для многопоточности)
    std::call_once(curl_global_init_flag, []() {
        CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (res != CURLE_OK) {
            throw std::runtime_error("curl_global_init() failed: " +
                                     std::string(curl_easy_strerror(res)));
        }
    });

    curl_handle_ = curl_easy_init();
    if (!curl_handle_) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    // Общие настройки
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &response_buffer_);
    curl_easy_setopt(curl_handle_, CURLOPT_FOLLOWLOCATION, 1L);      // следовать редиректам
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYPEER, 1L);      // ВКЛЮЧАЕМ проверку сертификатов (безопасно)
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYHOST, 2L);      // проверяем имя хоста
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, timeout_seconds_);
    curl_easy_setopt(curl_handle_, CURLOPT_USERAGENT, user_agent_.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_ACCEPT_ENCODING, "");      // авто-распаковка сжатых ответов
}

HttpRequest::~HttpRequest() {
    if (curl_handle_) {
        curl_easy_cleanup(curl_handle_);
    }
    // curl_global_cleanup() вызывать не нужно — она должна быть вызвана в конце main,
    // но мы её опускаем, т.к. она не обязательна (современные libcurl сами очищают при выгрузке)
}

void HttpRequest::setVerbose(bool verbose) {
    verbose_mode_ = verbose;
    curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, verbose_mode_ ? 1L : 0L);
}

void HttpRequest::setTimeout(long seconds) {
    timeout_seconds_ = seconds;
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, timeout_seconds_);
}

void HttpRequest::setUserAgent(const std::string& user_agent) {
    user_agent_ = user_agent;
    curl_easy_setopt(curl_handle_, CURLOPT_USERAGENT, user_agent_.c_str());
}

long HttpRequest::getLastHttpCode() const {
    return last_http_code_;
}

std::string HttpRequest::get(const std::string& url) {
    response_buffer_.clear();
    last_http_code_ = 0;

    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());

    CURLcode res = curl_easy_perform(curl_handle_);
    if (res != CURLE_OK) {
        throw std::runtime_error("CURL error: " +
                                 std::string(curl_easy_strerror(res)));
    }

    // Получаем HTTP-статус код
    long http_code = 0;
    curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &http_code);
    last_http_code_ = http_code;

    if (http_code >= 400) {
        // Сервер вернул ошибку, но мы всё равно возвращаем тело (можно и исключение бросить)
        // Для простоты просто сохраняем код, но не бросаем исключение.
    }

    return response_buffer_;
}

size_t HttpRequest::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* buffer = static_cast<std::string*>(userp);
    size_t total_size = size * nmemb;
    buffer->append(static_cast<char*>(contents), total_size);
    return total_size;
}
