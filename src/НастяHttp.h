#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <curl/curl.h>

class HttpRequest {
public:
    HttpRequest();
    ~HttpRequest();

    // Запрещаем копирование (RAII-объект)
    HttpRequest(const HttpRequest&) = delete;
    HttpRequest& operator=(const HttpRequest&) = delete;

    // Выполнить GET-запрос, вернуть тело ответа (бросает исключение при ошибке)
    std::string get(const std::string& url);

    void setVerbose(bool verbose);
    void setTimeout(long seconds);        // таймаут в секундах
    void setUserAgent(const std::string& user_agent);

    long getLastHttpCode() const;         // получить HTTP-статус последнего запроса

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

    CURL* curl_handle_;
    std::string response_buffer_;
    bool verbose_mode_;
    long timeout_seconds_;
    std::string user_agent_;
    long last_http_code_;
};

#endif // HTTP_REQUEST_H
