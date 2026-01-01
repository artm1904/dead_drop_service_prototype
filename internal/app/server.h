#pragma once

#include <memory>

#include "internal/delivery/http/handler.h"
#include "internal/domain/secret_manager.h"
#include "internal/server_types.h"
// #include "internal/delivery/http/router.h" // Removed separate router class to fix Bad Alloc

namespace internal {
namespace app {

class Server {
   public:
    void Run() {
        // Init Core Logic
        SecretManager secretManager;

        // Init Handlers
        SecretHandler handler(secretManager);

        // Init Web Server (Resource)
        AppType app;

        // --- Blueprints Setup (Inline) ---
        // We configure blueprints here because separating them into a function/class
        // causes std::bad_alloc due to Crow's internal template/reference handling issues.

        // 1. Blueprint for API
        crow::Blueprint api_bp("api");
        CROW_BP_ROUTE(api_bp, "/secret")
            .methods(crow::HTTPMethod::POST)(
                [&handler](const crow::request& req) { return handler.CreateSecret(req); });

        // Middleware attachment
        api_bp.CROW_MIDDLEWARES(app, LogMiddleware);
        app.register_blueprint(api_bp);

        // 2. Blueprint for Viewing
        crow::Blueprint view_bp("secret");
        CROW_BP_ROUTE(view_bp, "/<string>")
            .methods(crow::HTTPMethod::GET)(
                [&handler](const std::string& id) { return handler.GetSecret(id); });

        // Middleware attachment
        view_bp.CROW_MIDDLEWARES(app, LogMiddleware);
        app.register_blueprint(view_bp);

        // 3. Global Routes
        CROW_ROUTE(app, "/")([&handler]() { return handler.Index(); });
        CROW_ROUTE(app, "/favicon.ico")([&handler]() { return handler.Favicon(); });

        CROW_LOG_INFO << "Starting Dead Drop Service on port 8080...";

        // Run
        try {
            app.port(8080).multithreaded().run();
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Server crashed: " << e.what();
        }
    }
};

}  // namespace app
}  // namespace internal
