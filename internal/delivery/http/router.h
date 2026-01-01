#pragma once

#include "crow.h"
#include "internal/delivery/http/handler.h"

namespace internal {
namespace delivery {
namespace http {

class Router {
   public:
    // Factory method for API Blueprint
    static crow::Blueprint GetApiBlueprint(SecretHandler& handler) {
        crow::Blueprint api_bp("api");

        CROW_BP_ROUTE(api_bp, "/secret")
            .methods(crow::HTTPMethod::POST)(
                [&handler](const crow::request& req) { return handler.CreateSecret(req); });

        return api_bp;
    }

    // Factory method for View Blueprint
    static crow::Blueprint GetViewBlueprint(SecretHandler& handler) {
        crow::Blueprint view_bp("secret");

        CROW_BP_ROUTE(view_bp, "/<string>")
            .methods(crow::HTTPMethod::GET)(
                [&handler](const std::string& id) { return handler.GetSecret(id); });

        return view_bp;
    }
};

}  // namespace http
}  // namespace delivery
}  // namespace internal
