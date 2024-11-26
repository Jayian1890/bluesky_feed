//
// Created by Jared T on 11/26/24.
//

#include "../HttpsClient.cpp"

class getProfile : public JSON {
private:
    std::string _host = "https://public.api.bsky.app";
    std::string _endpoint = "/xrpc/app.bsky.actor.getProfile";
    std::string _bearerToken;
    std::string _actor;

public:
    explicit getProfile(std::string actor)
        : _actor(std::move(actor)) {
        this->fetch();
    }

    void setActor(std::string actor) {
        this->_actor = std::move(actor);
    }

    void fetch() {
        HttpsClient client;
        client.setHost(_host);
        client.setEndpoint(_endpoint);
        client.setBearerToken(_bearerToken);

        client.addQueryParam("actor", _actor);

        try {
            const JSON response = client.get();
            for (const auto& key : response.getKeys()) {
                set(key, response.get(key));
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching preferences: " << e.what() << "\n";
        }
    }
};