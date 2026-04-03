// === НОВЫЙ БЛОК — ОПРЕДЕЛЕНИЕ VPN ===
    std::cout << CYAN << " Мобильный : " << WHITE 
              << (mobile ? "Да" : "Нет") << RESET << std::endl;
    std::cout << CYAN << " Proxy / VPN : " << 
              (proxy ? BRIGHT_RED : WHITE) << (proxy ? "Да" : "Нет") << RESET << std::endl;
    std::cout << CYAN << " Hosting (дата-центр) : " << 
              (hosting ? BRIGHT_RED : WHITE) << (hosting ? "Да" : "Нет") << RESET << std::endl;

    std::cout << CYAN << " Статус : " 
              << (status == "success" ? BRIGHT_GREEN : BRIGHT_RED) 
              << status << RESET << std::endl;
    
    std::cout << BOLD << GREEN << "------------------------------------" << RESET << std::endl;
}
