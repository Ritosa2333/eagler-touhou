#pragma once

namespace Th09Overlay {
struct ConnectionStatus {
    const char* route = nullptr;
    unsigned player = 0;
    int rtt = -1;
    int variation = -1;
    bool probesUnanswered = false;
};
}
