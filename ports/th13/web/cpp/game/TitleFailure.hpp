#pragma once

#include <string>

namespace th13 {

struct TitleFailure {
    std::string error;

    bool fail(const std::string& reason) {
        if (error.empty()) error = reason;
        return false;
    }
};

}
