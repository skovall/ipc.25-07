// === НОВЫЕ ПОЛЯ ДЛЯ ОПРЕДЕЛЕНИЯ VPN ===
    bool mobile = false;   // мобильный интернет
    bool proxy = false;    // VPN / Proxy / Tor
    bool hosting = false;  // IP дата-центра (часто = VPN)

    // Конструктор по умолчанию
    GeoLocationData() 
        : ip("N/A"), country("N/A"), countryCode("N/A"), regionName("N/A"),
          city("N/A"), zip("N/A"), lat(0.0), lon(0.0),
          timezone("N/A"), isp("N/A"), org("N/A"), as("N/A"),
          status("N/A"), message("N/A"),
          mobile(false), proxy(false), hosting(false) {}
