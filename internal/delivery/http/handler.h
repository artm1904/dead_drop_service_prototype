#pragma once

#include <fstream>
#include <sstream>

#include "crow.h"
#include "internal/domain/secret_manager.h"

class SecretHandler {
   public:
    explicit SecretHandler(SecretManager& sm) : secretManager_(sm) {}

    // GET /
    crow::response Index() { return LoadTemplate("templates/index.html"); }

    // GET /favicon.ico
    crow::response Favicon() { return crow::response(""); }

    // POST /api/secret
    crow::response CreateSecret(const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);

        std::string secret = x["secret"].s();
        std::string id = secretManager_.CreateSecret(secret);

        crow::json::wvalue result;
        result["id"] = id;
        return crow::response(result);
    }

    // GET /secret/<id>
    crow::response GetSecret(const std::string& id) {
        auto secret = secretManager_.GetAndBurnSecret(id);
        if (!secret) {
            return crow::response(404, "Secret not found or already viewed.");
        }

        std::string page_content = LoadTemplateString("templates/secret.html");
        if (page_content.empty())
            return crow::response(500, "Internal Server Error: Template not found");

        std::string content = *secret;
        size_t pos = page_content.find("{{secret_content}}");
        if (pos != std::string::npos) {
            page_content.replace(pos, 18, content);
        }

        return crow::response(page_content);
    }

   private:
    SecretManager& secretManager_;

    // Helper to read file separate from routing logic
    crow::response LoadTemplate(const std::string& path) {
        std::string content = LoadTemplateString(path);
        if (content.empty())
            return crow::response(500, "Internal Server Error: Template not found");
        return crow::response(content);
    }

    std::string LoadTemplateString(const std::string& path) {
        std::ifstream t(path);
        if (!t.is_open()) return "";
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }
};
