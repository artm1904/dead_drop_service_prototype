#pragma once

#include <chrono>

#include "crow.h"

// Local Middleware (для Blueprints)
struct LogMiddleware : crow::ILocalMiddleware {
    struct context {
        std::chrono::steady_clock::time_point start_time;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        ctx.start_time = std::chrono::steady_clock::now();
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - ctx.start_time);
        CROW_LOG_INFO << "Request to " << req.url << " took " << duration.count() / 1000.0 << " ms";
    }
};

// Gloabl Middleware (для всего приложения)
struct LogMiddlewareGlobal {
    struct context {
        std::chrono::steady_clock::time_point start_time;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        ctx.start_time = std::chrono::steady_clock::now();
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - ctx.start_time);
        CROW_LOG_INFO << "GLOBAL:request to " << req.url << " took " << duration.count() / 1000.0
                      << " ms";
    }
};
