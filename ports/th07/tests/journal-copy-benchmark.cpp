// Isolate copy backend under the SAME legacy browser compiler target. No FPS
// claim: measure only copying identical live checkpoint bytes and undoing them.
#include <eagler/netplay/RollbackJournal.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

static void Check(bool ok) { if (!ok) std::abort(); }
int main()
{
    constexpr std::size_t Bytes = 4500000;
    constexpr unsigned Iterations = 96;
    using Clock = std::chrono::steady_clock;
    for (bool fast : {false, true})
    {
        Netplay::RollbackJournal journal;
        Check(journal.Reset({8, 8 * 1024 * 1024, 32, fast}));
        std::vector<unsigned char> world(Bytes, 0x5a);
        double captureMs = 0, restoreMs = 0;
        for (unsigned frame = 0; frame < Iterations + 8; ++frame)
        {
            const auto start = Clock::now();
            Check(journal.BeginFrame(frame));
            Check(journal.Touch(world.data(), world.size()));
            Check(journal.EndFrame());
            const auto captured = Clock::now();
            world[frame % Bytes] = 0x13;
            Check(journal.UndoTo(frame));
            const auto restored = Clock::now();
            Check(world[frame % Bytes] == 0x5a);
            if (frame >= 8)
            {
                captureMs += std::chrono::duration<double, std::milli>(captured - start).count();
                restoreMs += std::chrono::duration<double, std::milli>(restored - captured).count();
            }
        }
        std::printf("{\"backend\":\"%s\",\"bytes\":%zu,\"iterations\":%u,\"capture_ms\":%.4f,\"restore_ms\":%.4f}\n",
                    fast ? "bulk" : "wasm", Bytes, Iterations, captureMs / Iterations, restoreMs / Iterations);
    }
}
