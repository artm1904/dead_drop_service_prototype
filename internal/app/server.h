#pragma once

#include <memory>

#include "internal/delivery/http/handler.h"
#include "internal/delivery/http/router.h"
#include "internal/domain/fs_cash.h"
#include "internal/domain/secret_manager.h"
#include "internal/server_types.h"

namespace internal {
namespace app {

class Server {
   public:
    void Run() {
        // Init Core Logic
        SecretManager secretManager;

        ReadOnlyCache templates("templates");

        // Init Handlers
        SecretHandler handler(secretManager, templates);

        // Init Web Server (Stack allocated)
        AppType app;

        // --- Blueprints Setup ---

        // 1. API Blueprint (from Router Factory)
        auto api_bp = internal::delivery::http::Router::GetApiBlueprint(handler);
        api_bp.CROW_MIDDLEWARES(app, LogMiddleware);
        app.register_blueprint(api_bp);

        // 2. View Blueprint (from Router Factory)
        auto view_bp = internal::delivery::http::Router::GetViewBlueprint(handler);
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
