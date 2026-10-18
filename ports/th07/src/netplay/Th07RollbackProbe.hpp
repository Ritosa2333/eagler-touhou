#pragma once

namespace Netplay::Th07RollbackProbe
{
// Development-only runtime probe. With EAGLER_NETPLAY_ROLLBACK_PROBE=1
// (or the equivalent Web Module option), one stable gameplay tick is executed
// speculatively, restored, then committed again from the captured logical input.
// Normal TH_ENABLE_NETPLAY builds remain a direct RunCalcChain() pass-through.
int RunCalcChain();
} // namespace Netplay::Th07RollbackProbe
