#include "TitleFrame.hpp"
#include "DrawCapture.hpp"

namespace th13 {

TitleFrame::TitleFrame(TitleState& s, SessionState& p, TitleFrameServices& h, FrameScheduler& f, AnmManager& a)
    : state(s), progress(p), host(h), scheduler(f), animations(a) {
    callbacks[0].owner = callbacks[1].owner = this;
    callbacks[0].enabled = true;
    callbacks[1].enabled = true;
    callbacks[0].run = [](void* p) {
        auto& frame = *static_cast<TitleFrame*>(p);
        return frame.update(frame.input) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    callbacks[1].run = [](void* p) {
        auto& frame = *static_cast<TitleFrame*>(p);
        if (i32(frame.state.screen) == i32(TitleScreen::Replay) && !frame.host.draw_title_replay_texts())
            return i32(FrameAction::Error);
        if (i32(frame.state.screen) == i32(TitleScreen::MusicRoom) && !frame.host.draw_title_music_room_texts())
            return i32(FrameAction::Error);
        frame.submit_side_strips();
        return i32(FrameAction::Continue);
    };
    if (scheduler.add(callbacks[0], FramePass::Update, 6) < 0 || scheduler.add(callbacks[1], FramePass::Draw, 2) < 0)
        check(false, "Title callback registration failed");
}

TitleFrame::~TitleFrame() {
    for (auto& callback : callbacks)
        scheduler.remove(callback);
}

bool TitleFrame::check(bool ok, const char* why) {
    if (!ok && error.empty()) error = why;
    return ok;
}

bool TitleFrame::menu() { return check(host.update_title_menu(state.screen, input), "Title menu update failed"); }

bool TitleFrame::initialize() {
    if (!check(host.clear_title_overlay(), "Title text overlay release failed")) return false;
    const i32 reason = state.return_reason;
    auto main = [&]() {
        state.change_screen(TitleScreen::Main);
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    };
    if (reason == 4) {
        if (!check(host.restore_practice_selection(), "Practice selection restore failed")) return false;
        state.flags |= 2u;
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 5) {
        if (!check(host.restore_spell_practice_selection(), "Spell Practice selection restore failed")) return false;
        state.flags |= 2u;
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 2) {
        state.flags |= 2u;
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::Replay));
        state.menu.push();
        state.change_screen(TitleScreen::Replay);
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 3) {
        state.flags |= 2u;
        state.change_screen(TitleScreen::ResultName);
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 0) {
        state.flags &= ~2u;
        return main();
    }
    state.flags |= 2u;
    return main();
}

bool TitleFrame::update(const TitleFrameInput& value) {
    if (!error.empty()) return false;
    input = value;
    switch (i32(state.screen)) {
    case 0:
        if (!initialize()) return false;
        break;
    case 1:
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 2:
        if (state.screen == TitleScreen::Quit) {
            if (!check(host.title_exit(3), "Title exit destination failed")) return false;
            if (!check(host.fade_out_title_music(), "Title exit music fade failed")) return false;
        }
        break;
    case 6:
    case 7:
    case 8:
        if (!menu()) return false;
        break;
    case 10:
        if (!menu()) return false;
        break;
    case 17:
        if (!menu()) return false;
        break;
    case 3:
        if (!menu()) return false;
        break;
    case 4:
        if (!menu()) return false;
        break;
    case 11:
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 13:
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 14:
    case 15:
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 16:
        if (!menu()) return false;
        break;
    default:
        break;
    }
    state.age.tick(&progress.rate);
    return true;
}

void TitleFrame::submit_side_strips() { emit_title_side_strips(animations); }

void emit_title_side_strips(AnmManager& animations) {
    if (!draw_capture || !draw_capture->legacy_harness()) return;
    auto* text = animations.resource(0);
    if (!text) return;
    struct Strip {
        u32 entry, layer;
        float x0, x1;
    };
    const Strip strips[4] = {
        {2, 29, 127.5f, 511.5f}, {3, 30, 127.5f, 511.5f}, {2, 30, 127.5f, 511.5f}, {3, 20, 31.5f, 415.5f}};
    struct Vertex {
        float x, y, z, rhw;
        u32 diffuse;
        float u, v;
    };
    draw_capture->set_layout(touhou::graphics::VertexLayout::ScreenColorUv);
    auto& pipeline = draw_capture->pipeline();
    const auto tfactor_kept = pipeline.textureFactor;
    const auto source_kept = pipeline.sourceBlend;
    const auto destination_kept = pipeline.destinationBlend;
    const auto equation_kept = pipeline.blendEquation;
    pipeline.sourceBlend = touhou::graphics::BlendFactor::One;
    pipeline.destinationBlend = touhou::graphics::BlendFactor::Zero;
    pipeline.blendEquation = touhou::graphics::BlendEquation::Add;
    for (const auto& strip : strips) {
        draw_capture->bind_texture(draw_capture->texture(*text, strip.entry));
        draw_capture->set_stale_layer(i32(strip.layer));
        const Vertex verts[6] = {
            {strip.x0, 15.5f, 0, 1, 0xffffffffu, 0.2f, 1.f / 30.f},
            {strip.x1, 15.5f, 0, 1, 0xffffffffu, 0.8f, 1.f / 30.f},
            {strip.x0, 463.5f, 0, 1, 0xffffffffu, 0.2f, 29.f / 30.f},
            {strip.x1, 15.5f, 0, 1, 0xffffffffu, 0.8f, 1.f / 30.f},
            {strip.x0, 463.5f, 0, 1, 0xffffffffu, 0.2f, 29.f / 30.f},
            {strip.x1, 463.5f, 0, 1, 0xffffffffu, 0.8f, 29.f / 30.f},
        };
        draw_capture->primitives(touhou::graphics::Topology::Triangles, 2, verts, sizeof(Vertex));
    }
    pipeline.textureFactor = tfactor_kept;
    pipeline.sourceBlend = source_kept;
    pipeline.destinationBlend = destination_kept;
    pipeline.blendEquation = equation_kept;
}

}
