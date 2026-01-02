#pragma once

#include <fstream>
#include <sstream>

#include "crow.h"
#include "crow/http_response.h"
#include "internal/domain/read_only_cache.h"
#include "internal/domain/secret_manager.h"

class SecretHandler {
   public:
    explicit SecretHandler(SecretManager& sm, ReadOnlyCache& templates)
        : secretManager_(sm), templates_(templates) {}

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

        crow::response response(page_content);
        response.set_header("Content-Type", "text/html");
        return response;
    }

   private:
    SecretManager& secretManager_;
    ReadOnlyCache templates_;

    // Helper to read file separate from routing logic
    crow::response LoadTemplate(const std::string& path) {
        std::string content = LoadTemplateString(path);
        if (content.empty())
            return crow::response(500, "Internal Server Error: Template not found");
        crow::response response(content);
        response.set_header("Content-Type", "text/html");
        return response;
    }

    // std::string LoadTemplateString(const std::string& path) {
    //     std::ifstream t(path);
    //     if (!t.is_open()) return "";
    //     std::stringstream buffer;
    //     buffer << t.rdbuf();
    //     return buffer.str();
    // }

    std::string LoadTemplateString(const std::string& path) { return templates_.Get(path); }
};
