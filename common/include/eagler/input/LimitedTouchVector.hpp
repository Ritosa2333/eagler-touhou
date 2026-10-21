#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace eagler::input {

// Quantize an already radially limited touch vector to the nearest integer
// vector that stays inside the same speed budget. Component-wise round can
// move a boundary vector outside the budget, so choose the nearest feasible
// lattice point instead. Ties prefer the closest angle, then the greatest
// magnitude, then ascending coordinates for a stable result.
inline bool nearest_integer_touch_vector_within_speed(
    float x, float y, float speed, std::int32_t& out_x, std::int32_t& out_y) noexcept {
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(speed) || speed < 0.0f)
        return false;
    if (static_cast<double>(x) < std::numeric_limits<std::int32_t>::min() ||
        static_cast<double>(x) > std::numeric_limits<std::int32_t>::max() ||
        static_cast<double>(y) < std::numeric_limits<std::int32_t>::min() ||
        static_cast<double>(y) > std::numeric_limits<std::int32_t>::max()) return false;

    // Binary64 has the same arithmetic contract on native hosts and WASM.
    const double px = x, py = y, radius = speed;
    const auto floor_x = static_cast<std::int64_t>(std::floor(px));
    const auto floor_y = static_cast<std::int64_t>(std::floor(py));
    const double radius_square = radius * radius;
    const double source_square = px * px + py * py;
    double best_error = std::numeric_limits<double>::infinity();
    double best_angle = std::numeric_limits<double>::infinity();
    double best_square = -1.0;
    std::int64_t best_x = 0, best_y = 0;
    bool found = false;

    // The source is inside the speed disc. The nearest feasible lattice point
    // lies in this local neighborhood of its floor/ceiling coordinates.
    for (std::int64_t candidate_x = floor_x - 1; candidate_x <= floor_x + 2; ++candidate_x) {
        if (candidate_x < std::numeric_limits<std::int32_t>::min() ||
            candidate_x > std::numeric_limits<std::int32_t>::max()) continue;
        for (std::int64_t candidate_y = floor_y - 1; candidate_y <= floor_y + 2; ++candidate_y) {
            if (candidate_y < std::numeric_limits<std::int32_t>::min() ||
                candidate_y > std::numeric_limits<std::int32_t>::max()) continue;
            const double qx = static_cast<double>(candidate_x), qy = static_cast<double>(candidate_y);
            const double square = qx * qx + qy * qy;
            if (square > radius_square) continue;
            const double error_x = qx - px, error_y = qy - py;
            const double error = error_x * error_x + error_y * error_y;
            const double cross = qx * py - qy * px;
            const double angle = square > 0.0 ? cross * cross / square : source_square;
            if (!found || error < best_error ||
                (error == best_error && (angle < best_angle ||
                    (angle == best_angle && (square > best_square ||
                        (square == best_square && (candidate_x < best_x ||
                            (candidate_x == best_x && candidate_y < best_y)))))))) {
                found = true;
                best_error = error;
                best_angle = angle;
                best_square = square;
                best_x = candidate_x;
                best_y = candidate_y;
            }
        }
    }

    if (!found) return false;
    out_x = static_cast<std::int32_t>(best_x);
    out_y = static_cast<std::int32_t>(best_y);
    return true;
}

} // namespace eagler::input
