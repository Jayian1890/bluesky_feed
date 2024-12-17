#ifndef SETTINGS_TPP
#define SETTINGS_TPP

template <typename T>
T Settings::get(const std::string& key) {
    loadSettings();

    if (!json.contains(key)) {
        throw std::runtime_error("Key not found: " + key);
    }

    try {
        return json.at(key).get<T>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Failed to retrieve key '" + key + "' as the requested type: " + std::string(e.what()));
    }
}

template <typename T>
T Settings::get(const std::string& key, const T& defaultValue) {
    loadSettings();

    if (!json.contains(key)) {
        return defaultValue;
    }

    try {
        return json.at(key).get<T>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Failed to retrieve key '" + key + "' as the requested type: " + std::string(e.what()));
    }
}
#endif // SETTINGS_TPP
