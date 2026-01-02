#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>

class ReadOnlyCache {
   public:
    // Load all templates from directory
    explicit ReadOnlyCache(const std::string& path) {
        std::filesystem::path p(path);
        if (!std::filesystem::exists(p)) {
            throw std::runtime_error("Template directory missing");
        }

        for (const auto& entry : std::filesystem::directory_iterator(p)) {
            Load(entry.path().string());
        }
    }

    // Load template from file
    void Load(const std::string& path) {
        std::ifstream t(path);
        if (!t.is_open()) {
            throw std::runtime_error("Template missing");
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        templates_[path] = buffer.str();
    }

    std::string Get(const std::string& path) {
        if (templates_.find(path) == templates_.end()) {
            throw std::runtime_error("Template missing");
        }
        return templates_[path];
    }

   private:
    std::unordered_map<std::string, std::string> templates_;  // Путь к файлу -> Содержимое файла
};
