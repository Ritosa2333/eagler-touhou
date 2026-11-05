#include "../web/cpp/multiplayer/LiveMotionObservation.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include <cstdlib>

// Link-only support for the existing isolated rendering/resource tests.
// Production's inline enabled/capture_active flags stay false. These tests
// do not run an Application/SDL observer and do not validate live motion.
// Any actual observer callback is a test failure, never a silent no-op.
namespace th10::multiplayer::live_motion {
void CaptureQuad(const AnmVm&,const AnmVertex*,const RenderViewport&) noexcept {
    std::abort();
}
bool BeginBullet(const EnemyBullet&,const AnmVm&) noexcept {
    std::abort();
}
void EndObject() noexcept {
    std::abort();
}
}
#endif
