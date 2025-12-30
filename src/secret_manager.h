#ifndef SECRET_MANAGER_H
#define SECRET_MANAGER_H

#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>

class SecretManager {
   public:
    SecretManager() : rng_(std::random_device{}()) {}

    std::string CreateSecret(const std::string& text) {
        std::string id = GenerateID();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            secrets_[id] = text;
        }
        return id;
    }

    std::optional<std::string> GetAndBurnSecret(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = secrets_.find(id);
        if (it != secrets_.end()) {
            std::string content = it->second;
            secrets_.erase(it);  // Burn the secret
            return content;
        }
        return std::nullopt;
    }

   private:
    std::unordered_map<std::string, std::string> secrets_;  // Хранилище: ID -> Текст
    std::mutex mutex_;  // Защита от гонки потоков (веб-сервер crow многопоточный)
    std::mt19937 rng_;

    std::string GenerateID(int length = 8) {
        static const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

        std::string s(length, 0);
        for (int i = 0; i < length; ++i) {
            s[i] = charset[dist(rng_)];
        }
        return s;
    }
};

#endif  // SECRET_MANAGER_H
