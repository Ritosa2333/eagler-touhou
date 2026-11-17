#pragma once
#include "../../../portable/sdl/GraphicsState.hpp"
#include <cstddef>
#include <limits>

namespace th13 {
// State is the actual portable/sdl State in production, not a parallel device
// model. Own the small snapshot; borrow geometry only until both synchronous
// consumers return. Renderer owns any bytes it defers. Async observers must copy.
// Templating keeps native semantic tests independent of SDL/GLES headers.
template<class State> struct BasicRenderSubmission {
    enum class Kind { Draw, Clear, Target, Update, Prepare, Release, Present, Flush };
    Kind kind;
    State state;
    touhou::graphics::Topology topology = touhou::graphics::Topology::Triangles;
    uint32_t count = 0, stride = 0;
    const void* vertices = nullptr;
    bool direct = false;
    uint32_t flags = 0, color = 0, resource = 0, previous = 0;
    const int32_t* rect = nullptr;
    std::array<uint32_t, 4> region{};

    size_t vertex_bytes() const {
        using T = touhou::graphics::Topology;
        const size_t n = count;
        const size_t vertices = topology == T::Points ? n : topology == T::Lines ? n * 2 :
            topology == T::LineStrip ? n + 1 : topology == T::Triangles ? n * 3 : n + 2;
        return stride && vertices <= std::numeric_limits<size_t>::max() / stride ? vertices * stride : 0;
    }
    template<class Renderer> void consume(Renderer& renderer) const {
        using K = Kind;
        if (kind == K::Target || kind == K::Update || kind == K::Release) renderer.flush();
        renderer.state = state;
        switch (kind) {
        case K::Draw:
            if (direct) renderer.draw_batch(count, vertices, stride);
            else renderer.draw(topology, count, vertices, stride);
            break;
        case K::Clear: renderer.clear(flags, color, 1, 0, rect, rect ? 1 : 0); break;
        case K::Target: break;
        case K::Update: renderer.changed(resource, previous, region); break;
        case K::Prepare: renderer.prepare(resource); break;
        case K::Release: renderer.release(resource); break;
        case K::Present: renderer.present(resource); break;
        case K::Flush: renderer.flush(); break;
        }
    }
    template<class Renderer, class Observer, class Resolve>
    bool dispatch(Renderer& renderer, Observer& observer, Resolve resolve, void* owner) const {
        // Trace sees pre-operation resource versions. Failure must not suppress
        // or add production operations. No capture-only renderer/state exists.
        const bool ok = observer.consume(*this, resolve, owner);
        consume(renderer);
        return ok;
    }
};
}
