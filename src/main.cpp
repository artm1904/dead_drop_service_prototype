#include <fstream>
#include <sstream>

#include "crow.h"
#include "secret_manager.h"

std::string LoadTemplate(const std::string& path) {
    std::ifstream t(path);
    if (!t.is_open()) return "";
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

// Middleware for logging execution time
struct LogMiddleware {
    struct context {
        std::chrono::steady_clock::time_point start_time;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        ctx.start_time = std::chrono::steady_clock::now();
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        // Filter: ignore static files or specific routes
        if (req.url == "/favicon.ico") return;

        auto end_time = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - ctx.start_time);
        CROW_LOG_INFO << "Request to " << req.url << " took " << duration.count() / 1000.0 << " ms";
    }
};

int main() {
    // Define the app with middleware
    crow::App<LogMiddleware> app;
    SecretManager secretManager;

    // Serve main page
    CROW_ROUTE(app, "/")([]() {
        std::string page = LoadTemplate("templates/index.html");
        if (page.empty()) return crow::response(500, "Internal Server Error: Template not found");
        return crow::response(page);
    });

    CROW_ROUTE(app, "/favicon.ico")([] { return ""; });

    // Create secret API
    CROW_ROUTE(app, "/api/secret")
        .methods(crow::HTTPMethod::POST)([&secretManager](const crow::request& req) {
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            std::string secret = x["secret"].s();
            std::string id = secretManager.CreateSecret(secret);

            crow::json::wvalue result;
            result["id"] = id;
            return crow::response(result);
        });

    // View secret page
    CROW_ROUTE(app, "/secret/<string>")
    ([&secretManager](const std::string& id) {
        auto secret = secretManager.GetAndBurnSecret(id);
        if (!secret) {
            return crow::response(404, "Secret not found or already viewed.");
        }

        std::string page = LoadTemplate("templates/secret.html");
        if (page.empty()) return crow::response(500, "Internal Server Error: Template not found");

        // Simple template replacement
        std::string content = *secret;
        size_t pos = page.find("{{secret_content}}");
        if (pos != std::string::npos) {
            page.replace(pos, 18, content);
        }

        return crow::response(page);
    });

    app.port(8080).multithreaded().run();
}
