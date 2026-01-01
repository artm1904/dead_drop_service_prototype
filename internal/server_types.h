#pragma once

#include "crow.h"
#include "internal/middleware/logging.h"

// Определяем точный тип нашего приложения с middleware
using AppType = crow::App<LogMiddleware, LogMiddlewareGlobal>;
