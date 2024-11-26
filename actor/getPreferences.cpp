//
// Created by Jared T on 11/24/24.
//

#include <iostream>
#include <string>
#include "../HttpsClient.cpp"

class getPreferences : public JSON {
private:
    std::string preferencesEndpoint;
    std::string preferencesHost;
    std::string bearerToken;

public:
    getPreferences(const std::string& host, const std::string& endpoint, const std::string& token)
        : preferencesHost(host), preferencesEndpoint(endpoint), bearerToken(token) {}

    void fetch() {
        HttpsClient client;
        client.setHost(preferencesHost);
        client.setEndpoint(preferencesEndpoint);
        client.setBearerToken(bearerToken);

        try {
            JSON response = client.get();
            for (const auto& key : response.getKeys()) {
                JSON::set(key, response.get(key));
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching preferences: " << e.what() << "\n";
        }
    }

    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        try {
            return JSON::get(key);
        } catch (const std::out_of_range&) {
            return defaultValue;
        }
    }

    void set(const std::string& key, const std::string& value) {
        JSON::set(key, value);
    }
};

/*int main() {
    try {
        // Example setup
        getPreferences preferences(
            "https://jsonplaceholder.typicode.com",
            "/todos/1", // Example endpoint for demonstration
            "your-token-here" // Optional bearer token
        );

        // Fetch preferences from a remote endpoint
        preferences.fetch();

        // Access preferences
        std::cout << "User ID: " << preferences.get("userId", "Unknown") << "\n";
        std::cout << "Title: " << preferences.get("title", "No Title") << "\n";

        // Modify and generate updated preferences
        preferences.set("theme", "dark");
        std::cout << "Updated Preferences JSON: " << preferences.generate() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}*/