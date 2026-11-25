#pragma once

#include "ending_core.hpp"
#include "opening.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>

namespace soew {

struct EndingPlaybackInput {
    bool any_key = false;  // Original key_det (line fast-forward).
    bool gate_key = false; // Original key_delay groups translated by the host.
    std::optional<unsigned> music_measure; // Elapsed full measures; absent if no driver is available.
};

// An active but not-yet-reported song is at measure zero, not in fallback.
// The caller supplies elapsed foreground time only, and only for this song.
struct StaffMusicClock {
    static constexpr unsigned stall_ms = 5000;
    static constexpr unsigned measure_stall_ms = 15000;
    bool fallback = false;
    std::uint64_t last_frames = 0;
    unsigned last_measure = 0, frame_wait = 0, measure_wait = 0;
    std::optional<unsigned> update(bool active, std::uint64_t frames,
                                   std::optional<unsigned> measure, unsigned elapsed_ms) {
        if(fallback || !active){fallback=true;return {};}
        const auto add=[elapsed_ms](unsigned age,unsigned cap){return age+std::min(elapsed_ms,cap-age);};
        if(frames>last_frames){last_frames=frames;frame_wait=0;}
        else frame_wait=add(frame_wait,stall_ms);
        if(measure&&*measure>last_measure){last_measure=*measure;measure_wait=0;}
        else measure_wait=add(measure_wait,measure_stall_ms);
        if(frame_wait==stall_ms||measure_wait==measure_stall_ms){fallback=true;return {};}
        return last_measure;
    }
};

struct EndingRect { int x, y, width, height; };
inline constexpr EndingRect ending_picture_rect{160, 100, 320, 200};
inline constexpr EndingRect ending_line_rect{144, 332, 352, 16};
inline constexpr EndingRect ending_staff_clear_rect{416, 192, 224, 80};
inline constexpr EndingRect ending_quarter_rect(int quarter) {
    return {(quarter % 2) * 320, (quarter / 2) * 200, 320, 200};
}

// Commands are incremental: preserve the displayed page, the loaded picture and
// palettes between ticks. Assets, font rendering, audio and score UI belong to
// the host. Timed commands replace (rather than accompany) their source event.
// A sink need not interpret text or draw on ticks that have no commands.
struct EndingDrawStep {
    enum class Kind {
        Event, Tone, RevealText, ClearLine, ScrollRows, SlidePicture,
        TitleCel, MoveTitle, ClearStaffText, RotateStaff, FinishStaffRotate, VerdictMask
    } kind;
    EndingEvent event;
    int value = 0, aux = 0;
};
// Event: dispatch the original instant event (asset load, song, quarter, etc.).
// Tone: value is original 0..200; 0=black, 100=normal, 200=white.
// RevealText: TypeLine's value is exposed source bytes (44 max), asset/a select
// the original CP932 record; StaffText's value is the count of full-width glyphs.
// ClearLine / ClearStaffText: clear the respective rectangle above.
// ScrollRows: value is the source quarter's starting row, aux is row count.
// Move the current picture down by aux then copy these rows at the top.
// SlidePicture: move from aux (old X) to value (new X); original cuts four pixels
// from the right edge, so only the left 316 pixels of the new image remain.
// TitleCel: value is BFNT pattern base, aux is cel index. Logo left=432, top=184.
// MoveTitle: value is new logo Y; redraw the version string at (528, value+8).
// RotateStaff: value is the 0-based animation frame. event.a/b/c select quarter,
// speed and initial angle. The host draws original rotating rectangles.
// FinishStaffRotate: draw final outline (28,96)..(356,304), then the source
// quarter at (32,100) and set tone 200. The player waits four frames and restores 100.
// VerdictMask: value is mask index 0..2. The host applies the original mask to
// the indicated row (event.a), using script().verdict and the original labels.

class EndingPlayer {
public:
    explicit EndingPlayer(EndingScript script) : script_(std::move(script)) {}

    [[nodiscard]] const EndingScript& script() const { return script_; }
    [[nodiscard]] std::size_t event_index() const { return cursor_; }
    [[nodiscard]] bool finished() const { return cursor_ == script_.events.size() && delay_ == 0; }
    [[nodiscard]] bool staff_playing() const { return script_.staff.contains(cursor_); }

    // A display step has at most four Staff ticks. True tells the host to discard
    // any catch-up steps from the same timer batch before starting the verdict.
    template<class Sink>
    bool advance_display_frame(EndingPlaybackInput input, bool fast_staff, Sink&& sink) {
        if (!fast_staff || !staff_playing()) {
            tick(input, sink);
            return false;
        }
        for (unsigned i = 0; i < replay_view_speed(true); ++i) {
            tick_until(input, sink, script_.staff.end);
            if (!staff_playing()) return true;
        }
        return false;
    }

    // Replay Z controls only presentation pace. Ending gates are not recorded;
    // generate the same release/press samples per logical frame at either speed.
    template<class Sink>
    bool advance_replay_frame(bool held_z, std::optional<unsigned> music_measure, Sink&& sink) {
        for (unsigned i = 0; i < replay_view_speed(held_z) && !finished(); ++i) {
            const bool fast_staff = held_z && staff_playing();
            // Once accelerated Staff leaves real-time music behind, use the
            // scripted no-driver waits even after release. Audio still plays.
            replay_staff_frame_clock_ |= fast_staff;
            const EndingPlaybackInput input{false, (replay_frame_++ % 48 >= 24),
                replay_staff_frame_clock_ ? std::nullopt : music_measure};
            tick_until(input, sink, fast_staff ? script_.staff.end : script_.events.size());
            if (fast_staff && !staff_playing()) return true;
        }
        return false;
    }

    // Call once per logical frame. Multiple zero-delay commands can be emitted
    // in source order during a frame; process every command before presenting.
    template<class Sink>
    void tick(EndingPlaybackInput input, Sink&& sink) {
        tick_until(input, sink, script_.events.size());
    }

private:
    template<class Sink>
    void tick_until(EndingPlaybackInput input, Sink& sink, std::size_t end) {
        if (delay_ > 0 && --delay_ > 0) return;
        using K = EndingEventKind;
        using D = EndingDrawStep::Kind;
        while (cursor_ < end) {
            const auto& e = script_.events[cursor_];
            const auto draw = [&](D kind, int value = 0, int aux = 0) {
                sink(EndingDrawStep{kind, e, value, aux});
            };
            switch (e.kind) {
            case K::WaitFrames:
                wait(e.a);
                next();
                break;
            case K::WaitMeasure:
                if (input.music_measure) {
                    if (*input.music_measure < static_cast<unsigned>(e.a)) return;
                } else {
                    wait(e.b);
                }
                next();
                break;
            case K::WaitKey:
                if (step_ == 0) {
                    // Each sense is the OR of samples two frames apart.
                    gate_sample_ = input.gate_key;
                    step_ = 1;
                    wait(2);
                } else {
                    const bool down = gate_sample_ || input.gate_key;
                    if (gate_wait_press_ && down) next();
                    else {
                        if (!down) gate_wait_press_ = true;
                        step_ = 0;
                    }
                }
                break;
            case K::SetTone:
                draw(D::Tone, e.a);
                next();
                break;
            case K::Fade: {
                // master.lib first aligns to VSYNC, then shows 17 intermediate
                // levels spaced by the speed argument and finally the endpoint.
                if (step_ == 0) {
                    ++step_;
                    wait(1);
                    break;
                }
                constexpr std::array start{0, 100, 200, 100};
                constexpr std::array end{100, 0, 100, 200};
                const int source = start[e.a], target = end[e.a];
                if (step_ <= 17) {
                    draw(D::Tone, source + (step_ - 1) * ((target > source) ? 6 : -6));
                    ++step_;
                    wait(e.b);
                } else {
                    draw(D::Tone, target);
                    next();
                }
                break;
            }
            case K::TypeLine:
            case K::StaffText: {
                const bool staff = (e.kind == K::StaffText);
                if (staff && e.a != int(EndingStaffText::Title)) {
                    draw(D::ClearStaffText);
                    draw(D::Event);
                    next();
                    break;
                }
                constexpr int staff_title_glyphs = 10; // 封魔録 + two full-width spaces + ＳＴＡＦＦ.
                const int glyphs = staff ? staff_title_glyphs : int(EndingText::line_bytes / 2);
                const bool accelerate = e.fast_and_clear && input.any_key;
                if (step_ < glyphs) {
                    draw(D::RevealText, (step_ + 1) * (staff ? 1 : 2));
                    wait(accelerate ? ((step_ & 1) ? e.b / 3 : 0) : e.b);
                    ++step_;
                } else if (step_ < glyphs + 20) {
                    // ReC98 end.cpp:195–224: 20 two-frame holds, or frame_delay(0).
                    ++step_;
                    if (!accelerate) wait(2);
                } else {
                    if (e.fast_and_clear) draw(D::ClearLine);
                    next();
                }
                break;
            }
            case K::ScrollMarisa:
            case K::SlideToStaff:
            case K::StaffTitleDown:
                if (step_ == e.c) {
                    next();
                    break;
                }
                if (e.kind == K::ScrollMarisa) draw(D::ScrollRows, 200 - e.b * (step_ + 1), e.b);
                else if (e.kind == K::SlideToStaff) draw(D::SlidePicture, e.a - e.b * (step_ + 1), e.a - e.b * step_);
                else draw(D::MoveTitle, e.a + e.b * step_);
                ++step_;
                wait(1);
                break;
            case K::StaffTitleCels:
                if (step_ == e.a) next();
                else {
                    draw(D::TitleCel, e.c * step_, step_);
                    ++step_;
                    wait(e.b);
                }
                break;
            case K::StaffRotate:
                if (step_ < 20) {
                    draw(D::RotateStaff, step_++);
                    wait(1);
                } else if (step_ == 20) {
                    draw(D::FinishStaffRotate);
                    ++step_;
                    wait(4);
                } else {
                    draw(D::Tone, 100);
                    next();
                }
                break;
            case K::VerdictRow:
            case K::VerdictSkillValue:
                if (step_ == 3) next();
                else {
                    draw(D::VerdictMask, step_++);
                    wait(10);
                }
                break;
            case K::ClearLine:
                draw(D::ClearLine);
                next();
                break;
            default:
                draw(D::Event);
                next();
                break;
            }
            if (delay_ > 0) return;
        }
    }

    EndingScript script_;
    std::size_t cursor_ = 0;
    int step_ = 0, delay_ = 0;
    bool gate_sample_ = false, gate_wait_press_ = false;
    unsigned replay_frame_ = 0;
    bool replay_staff_frame_clock_ = false;

    void wait(int frames) { delay_ = frames; }
    void next() {
        ++cursor_;
        step_ = 0;
        gate_wait_press_ = false;
    }
};

} // namespace soew
