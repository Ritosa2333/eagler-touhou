#include "ending_renderer.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
using namespace soew;
using K = EndingEventKind;
using F = EndingFade;
using I = EndingInk;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::vector<std::byte> read_asset(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    const auto size = file.tellg();
    require(file.good() && size > 0, "Missing ending asset");
    std::vector<std::byte> bytes(static_cast<std::size_t>(size));
    file.seekg(0);
    require(static_cast<bool>(file.read(reinterpret_cast<char*>(bytes.data()), size)), "Failed reading ending asset");
    return bytes;
}

void check_text(const std::filesystem::path& root) {
    constexpr std::array names{"END1.TXT", "END2.TXT", "END3.TXT"};
    constexpr std::array counts{51u, 94u, 12u};
    for (std::size_t i = 0; i < names.size(); ++i) {
        auto bytes = read_asset(root / names[i]);
        const auto file = EndingText::decode(bytes, names[i]);
        require(file.lines.size() == counts[i], "Unexpected original record count");
        for (std::size_t row = 0; row < counts[i]; ++row) {
            require(std::equal(file.lines[row].begin(), file.lines[row].end(), bytes.begin() + row * 46),
                "Original CP932 content was modified");
        }
        require(file.lines.front()[0] == std::byte{0x81}, "Unexpected original opening byte");
        const auto rejects = [&](std::span<const std::byte> raw, std::string_view name) {
            try { (void)EndingText::decode(raw, name); }
            catch (const std::runtime_error&) { return true; }
            return false;
        };
        require(rejects(std::span(bytes).first(bytes.size() - 1), names[i]), "Accepted incomplete record");
        require(rejects(bytes, "UNRELATED.TXT"), "Accepted unrecognized asset");
        bytes[45] = std::byte{0};
        require(rejects(bytes, names[i]), "Accepted broken CRLF");
    }
}

std::vector<EndingEvent> select(const EndingScript& script, K kind) {
    std::vector<EndingEvent> result;
    for (const auto& e : script.events) if (e.kind == kind) result.push_back(e);
    return result;
}

std::vector<int> args(const EndingScript& script, K kind) {
    std::vector<int> result;
    for (const auto& e : script.events) if (e.kind == kind) result.push_back(e.a);
    return result;
}

void check_standard(const std::filesystem::path& root) {
    constexpr std::array good_starts{27, 48, 71}, good_ends{47, 70, 93};
    constexpr std::array bad_starts{27, 34, 42}, bad_ends{33, 41, 50};
    constexpr std::array measure_order{5, 6, 8, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57};
    for (bool bad : {false, true}) {
        for (unsigned shot = 0; shot < 3; ++shot) {
            const auto script = make_ending_script({.continues_used = bad ? 2u : 0u, .shot_type = shot});
            require(script.branch == (bad ? EndingBranch::Bad : EndingBranch::Good), "Wrong continue branch");
            for (const auto& e : script.events) {
                if (!e.asset.empty()) require(std::filesystem::is_regular_file(root / e.asset), "Event refers to nonexistent asset");
            }
            const auto file = bad ? "END1.TXT" : "END2.TXT";
            const auto texts = select(script, K::LoadText);
            require(texts == std::vector<EndingEvent>{{K::LoadText, file}, {K::LoadText, "END3.TXT"}}, "Wrong text order");
            require(select(script, K::PlaySong) == std::vector<EndingEvent>{{K::PlaySong,"END1.M"},{K::PlaySong,"ENDING.M"}},
                "Wrong song order");
            std::vector<int> expected;
            for (int row = 0; row <= 26; ++row) expected.push_back(row);
            const auto final_row = bad ? bad_ends[shot] : good_ends[shot];
            for (int row = bad ? bad_starts[shot] : good_starts[shot]; row <= final_row; ++row) expected.push_back(row);
            require(args(script, K::TypeLine) == expected, "Missing, duplicated or cross-route text row");
            const auto lines = select(script, K::TypeLine);
            const auto final = lines.back();
            require(final.asset == file && final.a == final_row && final.b == (bad ? 9 : 12) && !final.fast_and_clear,
                "Incorrect final line hold");
            for (auto it = lines.begin(); it != lines.end() - 1; ++it) {
                require(it->asset == file && it->fast_and_clear, "Incorrect ordinary line flags");
            }
            auto typed = [&](int row) -> const EndingEvent& {
                const auto it = std::find_if(lines.begin(), lines.end(), [&](const auto& e) { return e.a == row; });
                if (it == lines.end()) throw std::runtime_error("Test referenced row from another route");
                return *it;
            };
            const auto pictures = select(script, K::LoadPicture);
            const auto quarters = select(script, K::ShowQuarter);
            if (bad) {
                require(typed(6).ink == I::Yellow && typed(7).ink == I::White && typed(24).ink == I::Yellow,
                    "Incorrect ink for bad ending");
                require(lines[lines.size() - 2].b == 12, "Missing pre-final slowdown");
                require(pictures[1].asset == "ED02.PI" && quarters[3].a == std::array{0, 2, 3}[shot],
                    "Incorrect shot-specific bad image");
                require(select(script, K::ScrollMarisa) == std::vector<EndingEvent>{{K::ScrollMarisa, {}, 3, 2, 100}},
                    "Missing original Marisa scroll");
            } else {
                require(typed(14).ink == I::Green && typed(16).ink == I::White && typed(17).ink == I::Green,
                    "Incorrect ink for good ending");
                require(pictures[1].asset == "ED03.PI" && pictures[2].asset == "ED04.PI" && pictures[3].asset == "ED05.PI" &&
                    quarters[9].a == int(shot), "Incorrect shot-specific good image");
                require(select(script, K::LoadPalette).front().asset == "ED03A.RGB", "Missing grayscale palette");
                require(select(script, K::ScrollMarisa).empty(), "Bad ending scroll leaked to good ending");
            }
            const auto last = std::find(script.events.begin(), script.events.end(), final);
            require(*(last + 1) == EndingEvent{K::LoadText,"END3.TXT"} && *(last + 2) == EndingEvent{K::WaitFrames,{},30} &&
                *(last + 3) == EndingEvent{K::Fade,{},int(F::WhiteOut),1} && *(last + 4) == EndingEvent{K::PlaySong,"ENDING.M"} &&
                *(last + 5) == EndingEvent{K::ClearLine}, "Incorrect transition from ending to staff roll");
            require(args(script, K::WaitMeasure) == std::vector<int>(measure_order.begin(), measure_order.end()),
                "Incorrect staff cue order");
            for (const auto& cue : select(script, K::WaitMeasure)) require(cue.b == 100, "Missing no-BGM fallback");
            require(select(script, K::SlideToStaff) == std::vector<EndingEvent>{{K::SlideToStaff,{},160,4,31}},
                "Incorrect staff slide extent");
            require(select(script, K::StaffTitleCels) == std::vector<EndingEvent>{{K::StaffTitleCels,"ENDFT.BFT",11,4,5}},
                "Incorrect staff logo animation");
            require(select(script, K::StaffRotate) == std::vector<EndingEvent>{
                {K::StaffRotate,{},0,4,0x29},{K::StaffRotate,{},2,-4,0x29},{K::StaffRotate,{},3,4,0x29},
                {K::StaffRotate,{},0,-4,0x29},{K::StaffRotate,{},1,8,-0x17},{K::StaffRotate,{},2,-8,-0x17},
                {K::StaffRotate,{},3,8,-0x17},{K::StaffRotate,{},0,-8,-0x17},{K::StaffRotate,{},1,12,0x29},
                {K::StaffRotate,{},2,-12,0x29},{K::StaffRotate,{},3,12,0x29}}, "Incorrect staff rotation sequence");
            require(args(script, K::StaffText) == std::vector<int>{0,1,2,3,4}, "Incorrect staff text order");
            require(select(script, K::VerdictBackdrop) == std::vector<EndingEvent>{{K::VerdictBackdrop,"ED09.PI"}},
                "Wrong verdict backdrop");
            require(args(script, K::VerdictRow) == std::vector<int>{0,1,2,3,4,5,6,8}, "Incorrect verdict row order");
            require(select(script, K::VerdictSkillValue) == std::vector<EndingEvent>{{K::VerdictSkillValue,"END3.TXT",6,11}},
                "Unlinked skill verdict text");
            require(select(script, K::RegisterScore).size() == 1, "Missing score registration handoff");
        }
    }
}

void check_verdict_and_extra() {
    require(ending_calculate_skill(0, 0, 0, 0, 0) == 60 &&
        ending_calculate_skill(1, 1, 2, 3, 9) == 77 &&
        ending_calculate_skill(3, 0, 0, 0, 40) == 145 &&
        ending_calculate_skill(0, 3, 20, 25, -10) == 1 &&
        ending_calculate_skill(0, 4, 17, 0, 1) == -1,
        "Incorrect original skill formula or clamp");
    require(ending_resident_score(12345, 2) == 123452 && ending_resident_score(0, 0) == 0 &&
        ending_resident_score(0xffffffffu, 5) == 0xfffffffbu,
        "Incorrect verdict score handoff");
    const int raw = ending_calculate_skill(1, 1, 2, 3, 9);
    require(ending_verdict({.skill = raw}).title_line == 3, "Original skill did not reach special 77 verdict");
    for (int skill = -3; skill < 104; ++skill) {
        const int clamped = std::clamp(skill, 0, 100);
        constexpr std::array low_to_high{11,10,9,8,7,6,5,4,2,1,0};
        const auto v = ending_verdict({.continues_used = 2, .rank = 3, .score = 12345, .start_lives = 4, .start_bombs = 5, .skill = skill});
        require(v.score == 12345 && v.continues_used == 2 && v.rank == 3 && v.initial_players == 5 &&
            v.initial_bombs == 5 && v.skill == clamped && v.title_line == (clamped == 77 ? 3 : low_to_high[clamped / 10]),
            "Incorrect verdict value or title threshold");
    }
    const auto unlocked = make_ending_script({.extra_unlocked = true});
    require(select(unlocked, K::FullPicture) == std::vector<EndingEvent>{{K::FullPicture,"ALL.PI"},{K::FullPicture,"BUT.PI"}},
        "Missing unlocked standard ending card");
    require(select(make_ending_script({.continues_used = 1, .extra_unlocked = true}), K::FullPicture).empty(),
        "Unlock card should not follow a continued standard ending");
    for (bool extra_unlocked : {false, true}) {
        const auto extra = make_ending_script({.continues_used = 1, .rank = 4, .extra_unlocked = extra_unlocked});
        require(extra.branch == EndingBranch::Extra && select(extra, K::TypeLine).empty() &&
            select(extra, K::StaffText).empty() && select(extra, K::VerdictRow).empty(), "Extra leaked into standard flow");
        const auto cards = select(extra, K::FullPicture);
        require(extra_unlocked ? cards == std::vector<EndingEvent>{{K::FullPicture,"ALL.PI"},{K::FullPicture,"EXTRA.PI"}} : cards.empty(),
            "Wrong Extra result cards");
    }
    require(ending_staff_lines(EndingStaffText::Graphics).size() == 3 && ending_staff_lines(EndingStaffText::TestPlayers).back().y == 296 &&
        ending_verdict_rank[3] == "LUNATIC" && ending_verdict_labels[7].empty(), "Missing original credit layout or verdict labels");
}

void check_staff_music_clock(){
    StaffMusicClock clock;
    require(clock.update(true,0,{},0)==0, "Prefill must not start the fallback timer");
    require(clock.update(true,960,4,40)==4, "Completed measure was not published");
    require(clock.update(true,960,{},0)==4, "Paused wall time consumed the watchdog");
    require(clock.update(true,1920,3,20)==4, "Retirement watermark regressed");
    require(clock.update(true,1920,4,StaffMusicClock::stall_ms-1)==4, "Premature stall fallback");
    require(!clock.update(true,1920,4,1), "Stalled output did not fall back");
    require(!clock.update(true,2880,9,0), "Fallback should latch through the rest of Staff");
    clock={};
    require(!clock.update(false,0,{},0), "Unavailable device did not select fallback");
    clock={};
    require(clock.update(true,960,1,0)==1, "Unexpected first measure");
    require(!clock.update(true,1920,1,StaffMusicClock::measure_stall_ms), "Song never reached its next measure");

    EndingScript script;
    script.events={{K::WaitMeasure,{},5,100},{K::StaffVersion},{K::WaitFrames,{},12},
                   {K::WaitMeasure,{},6,100},{K::StaffVersion},{K::VerdictBackdrop}};
    script.staff.begin=0;script.staff.end=5;
    EndingPlayer player(std::move(script));
    unsigned versions=0,verdicts=0;
    const auto sink=[&](const auto& step){versions+=step.event.kind==K::StaffVersion;verdicts+=step.event.kind==K::VerdictBackdrop;};
    for(int i=0;i<120;++i)player.advance_display_frame({false,false,4},true,sink);
    require(versions==0&&player.event_index()==0, "Fast-forward crossed an unheard cue");
    for(int i=0;i<8;++i)player.advance_display_frame({false,false,5},true,sink);
    require(versions==1&&player.event_index()==3, "Fast-forward did not stop at the next cue");
    player.advance_display_frame({false,false,5},false,sink);
    require(versions==1&&verdicts==0, "Release applied queued fast-forward steps");
    require(player.advance_display_frame({false,false,6},true,sink)&&versions==2&&verdicts==0,
            "Staff crossed the Verdict boundary in the fast display batch");
    player.advance_display_frame({false,false,6},false,sink);
    require(verdicts==1, "Verdict did not start on the next display step");
}

void check_staff_playback(const EndingRun& run, bool replay) {
    const auto script = make_ending_script(run);
    const bool staff = script.branch != EndingBranch::Extra;
    require(staff ? (script.staff.begin < script.staff.end &&
        script.events.at(script.staff.begin) == EndingEvent{K::PlaySong, "ENDING.M"} &&
        script.events.at(script.staff.end).kind == K::VerdictBackdrop) :
        script.staff.begin == script.staff.end, "Wrong Staff range");
    EndingPlayer ordinary(script), viewer(script);
    std::vector<EndingDrawStep> expected, actual;
    const auto record = [&](const auto& step) { expected.push_back(step); };
    const auto render = [&](const auto& step) { actual.push_back(step); };
    const auto equal = [](const auto& a, const auto& b) {
        return a.kind == b.kind && a.event == b.event && a.value == b.value && a.aux == b.aux;
    };
    unsigned frame = 0;
    unsigned held_staff_frames = 0, released_staff_frames = 0;
    bool reached_verdict = false, previous_boundary = false, tested_gate = false;
    for (; frame < 30000 && !viewer.finished(); ++frame) {
        const bool playing_staff = viewer.staff_playing();
        const auto staff_frame = held_staff_frames + released_staff_frames;
        // Begin Staff at normal speed, hold, release briefly, then hold
        // through the first verdict gate. Replay supplies its own gate.
        const bool held = !playing_staff || (staff_frame >= 4 && (staff_frame < 60 || staff_frame >= 64));
        const bool at_gate = viewer.event_index() < script.events.size() &&
            script.events[viewer.event_index()].kind == K::WaitKey;
        if (at_gate && staff && !replay && !tested_gate) {
            const auto gate_index = viewer.event_index();
            for (int t = 0; t < 64; ++t) viewer.advance_display_frame({true, true, {}}, true, render);
            require(viewer.event_index() == gate_index, "Held Staff Z bypassed the verdict gate");
            for (int t = 0; t < 8; ++t) viewer.advance_display_frame({false, false, {}}, false, render);
            require(viewer.event_index() == gate_index, "Release alone bypassed the verdict gate");
            tested_gate = true;
            // The ordinary player has the same scripted gate and sees the same re-arm.
            for (int t = 0; t < 64; ++t) ordinary.tick({true, true, {}}, record);
            for (int t = 0; t < 8; ++t) ordinary.tick({false, false, {}}, record);
        }
        const bool gate = at_gate ? (replay ? frame % 48 >= 24 : frame % 16 >= 8) : true;
        const EndingPlaybackInput input{!replay, gate, std::nullopt};
        const bool boundary = viewer.advance_display_frame(input, held, render);
        const int ticks = held && playing_staff ? 4 : 1;
        held_staff_frames += held && playing_staff;
        released_staff_frames += !held && playing_staff;
        // The baseline tick may enter the verdict on the same logical step as
        // the final Staff tone. Strip the commands for that suffix on this frame;
        // check them together again after the next normal display frame.
        if (previous_boundary) {
            require(!boundary, "Verdict entered Staff again");
        } else {
            for (int i = 0; i < ticks && !ordinary.finished(); ++i) {
                ordinary.tick(input, record);
                if (playing_staff && !ordinary.staff_playing()) break;
            }
        }
        if (boundary) {
            require(!reached_verdict && !viewer.staff_playing() && viewer.event_index() == script.staff.end &&
                ordinary.event_index() > script.staff.end && expected.size() == actual.size() + 1 &&
                expected.back().event.kind == K::VerdictBackdrop,
                "Fast batch borrowed steps from the verdict");
            reached_verdict = true;
        } else {
            require(ordinary.event_index() == viewer.event_index() && expected.size() == actual.size(),
                "Wrong presentation frame budget");
        }
        previous_boundary = boundary;
    }
    require(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end(), equal),
        "Staff changed incremental command order");
    require(viewer.finished() && ordinary.finished(), "Ending did not finish");
    require(staff ? (reached_verdict && held_staff_frames > 0 && released_staff_frames > 0 && (replay || tested_gate)) :
        (!reached_verdict && !held_staff_frames && !released_staff_frames), "Unexpected Staff playback coverage");
    const auto emitted = [&](K kind) {
        return std::count_if(actual.begin(), actual.end(), [=](const auto& s) {
            return s.kind == EndingDrawStep::Kind::Event && s.event.kind == kind;
        });
    };
    require(emitted(K::PlaySong) == (staff ? 2 : 0) && emitted(K::VerdictBackdrop) == (staff ? 1 : 0) &&
        emitted(K::RegisterScore) == 1, "Duplicated or skipped song, verdict, or registration handoff");
}
// Endings have no replay events. Holding the viewer control must preserve the
// authored draw sequence and complete the release/press gates on its own.
void check_replay_ending_speed(const EndingRun& run) {
    const auto script = make_ending_script(run);
    const bool staff = script.staff.begin < script.staff.end;
    using Draw = EndingDrawStep;
    const auto collect = [](auto& steps){ return [&](const Draw& s){ steps.push_back(s); }; };
    const auto equal = [](const Draw& a, const Draw& b){
        return a.kind == b.kind && a.event == b.event && a.value == b.value && a.aux == b.aux;
    };
    std::vector<Draw> expected;
    EndingPlayer reference(script);
    unsigned frames = 0;
    while (frames < 30000 && !reference.finished())
        reference.tick({false, frames++ % 48 >= 24, std::nullopt}, collect(expected));
    require(reference.finished(), "Reference ending did not pass its auto-acknowledgement gates");
    for (bool mixed : {false, true}) {
        EndingPlayer viewer(script);
        std::vector<Draw> actual;
        unsigned callbacks = 0, staff_exit = 0;
        while (callbacks < frames && !viewer.finished()) {
            // Pause the hold during Staff and again during the verdict/Extra card.
            const bool held = !mixed || (callbacks % 80 < 60);
            const bool boundary = viewer.advance_replay_frame(held, std::nullopt, collect(actual));
            if (boundary) {
                ++staff_exit;
                require(viewer.event_index() == script.staff.end && actual.back().kind == Draw::Kind::Tone,
                    "Replay Staff processed a verdict frame in the same fast batch");
            }
            ++callbacks;
        }
        require(viewer.finished() && callbacks <= frames / 2, "Held Z failed to accelerate the whole replay ending");
        require(mixed || staff_exit == unsigned(staff), "Replay missed the Staff presentation boundary");
        require(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end(), equal),
            "Held Z changed the replay ending's draw or gate sequence");
    }
}

void check_replay_staff_audio_release() {
    EndingScript script;
    script.events={{K::WaitMeasure,{},5,100},{K::StaffVersion},{K::WaitMeasure,{},6,100},
        {K::StaffVersion},{K::WaitFrames,{},2},{K::VerdictBackdrop}};
    script.staff = {0,5};
    EndingPlayer viewer(script);
    unsigned versions = 0, verdicts = 0;
    const auto sink = [&](const auto& s){
        versions += s.event.kind == K::StaffVersion;
        verdicts += s.event.kind == K::VerdictBackdrop;
    };
    for (int i = 0; i < 120; ++i) viewer.advance_replay_frame(false, 4, sink);
    require(versions == 0 && viewer.event_index() == 0, "Normal replay Staff stopped following the soundtrack");
    for (int i = 0; i < 26; ++i) viewer.advance_replay_frame(true, 4, sink);
    require(versions == 1 && viewer.event_index() == 3, "Held replay Z stalled at a Staff music cue");
    for (int i = 0; i < 96; ++i) viewer.advance_replay_frame(false, 4, sink);
    require(versions == 1, "Release did not immediately return to one logical frame per display step");
    viewer.advance_replay_frame(false, 4, sink);
    require(versions == 2 && verdicts == 0, "Release waited for stale Staff music position");
    viewer.advance_replay_frame(true, 4, sink);
    require(verdicts == 1, "Replay did not resume the verdict after Staff");
}
} // namespace

int main(int argc, char** argv) {
    try {
        require(argc <= 3, "Pass the original-asset directory and optional locale root, or no arguments");
        check_verdict_and_extra();
        check_staff_music_clock();
        check_replay_staff_audio_release();
        check_replay_ending_speed({.extra_unlocked = true});
        check_replay_ending_speed({.continues_used = 1});
        check_replay_ending_speed({.rank = 4, .extra_unlocked = true});
        require(std::wstring_view(text::staff_fast_forward(Language::Japanese)) == L"Z長押し：4倍速" &&
            std::wstring_view(text::staff_fast_forward(Language::English)) == L"Hold Z: 4× speed" &&
            std::wstring_view(text::staff_fast_forward(Language::ChineseSimplified)) == L"按住 Z：4 倍速" &&
            std::wstring_view(text::staff_fast_forward(Language::ChineseTraditional)) == L"按住 Z：4 倍速",
            "Missing translated hold-Z hint");
        for (bool replay : {false, true}) {
            check_staff_playback({.extra_unlocked = true}, replay);
            check_staff_playback({.continues_used = 1}, replay);
            check_staff_playback({.rank = 4, .extra_unlocked = true}, replay);
        }
        if (argc >= 2) {
            check_text(argv[1]);
            check_standard(argv[1]);
        }
        if(argc==3){
            const std::filesystem::path root=argv[2];
            validate_locale_assets(root);
            require(has_locale_assets(root), "Locale overlay is missing");
            for(const auto name:{"END1.TXT","END2.TXT","END3.TXT"}){
                const auto ja=EndingText::decode(read_asset(locale_asset(root,Language::Japanese,name)),name);
                const auto en=EndingText::decode(read_asset(locale_asset(root,Language::English,name)),name);
                require(ja.lines.size()==en.lines.size()&&ja.lines!=en.lines,"Ending variants share structure, not text");
            }
            for(const auto lang:{Language::Japanese,Language::English}){
                require(ending_staff_lines(EndingStaffText::Title,lang).front().text.size()==30,"Staff title reveal width differs");
                require(ending_staff_lines(EndingStaffText::Graphics,lang).size()==3,"Missing graphics credits");
            }
            std::cout<<"localized endings: 3 assets and both staff layouts verified\n";
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
