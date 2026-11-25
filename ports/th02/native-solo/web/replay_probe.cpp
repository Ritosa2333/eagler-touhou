#include "replay_session.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: soew-replay-probe <content> <output.rep>\n";
        return 1;
    }
    const std::filesystem::path root = argv[1];
    try {
        soew::NativeReplayConfig config;
        config.seed = 0x54483032;
        soew::NativeReplayRecorder recorder(config);
        soew::ReplaySession session(config);
        auto scene = soew::load_replay_scene(root, config);
        std::cout << "[\"" << soew::replay_state_digest(scene) << '"';
        for (unsigned tick = 0; tick < 900; ++tick) {
            unsigned input = tick % 16 < 7 ? soew::Shot : 0;
            switch ((tick / 50) % 4) {
                case 0: input |= soew::Left; break;
                case 2: input |= soew::Right; break;
            }
            if (tick % 120 < 60) input |= soew::Focus;
            if (tick == 480) input |= soew::Bomb;
            const auto step = session.tick(scene, soew::normalize_replay_tick(input, input & soew::Shot));
            if (step.status != soew::ReplayStepStatus::Running || !step.consumed_tick ||
                step.stage_advanced || !recorder.record(step.event))
                throw std::runtime_error("Stage 1 replay probe stopped at " + std::to_string(tick) + ": " + step.error);
            std::cout << ",\"" << soew::replay_state_digest(scene) << '"';
        }
        if (!recorder.finalize(session.stop(scene))) throw std::runtime_error("Cannot finalize replay");
        const auto bytes = soew::native_replay_detail::encode(recorder.replay());
        std::ofstream out(argv[2], std::ios::binary);
        out.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
        out.close();
        if (!out) throw std::runtime_error("Cannot write replay");
        std::cout << "]\n";
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
