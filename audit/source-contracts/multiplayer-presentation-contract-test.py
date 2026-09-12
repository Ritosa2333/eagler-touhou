from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PLAYER = (ROOT / "src/Player.cpp").read_text(encoding="utf-8")
ANM_VM = (ROOT / "src/AnmVm.hpp").read_text(encoding="utf-8")


# TH06's AnmVm has one current/previous color pair. Do not mechanically import
# TH07's color2/prevColor2 rule into a layout that does not contain those fields.
assert "ZunColor color;" in ANM_VM
assert "ZunColor prevColor;" in ANM_VM
assert "color2" not in ANM_VM
assert "prevColor2" not in ANM_VM

# Remote smoothing is presentation-owned. It receives the already interpolated
# logical draw target and returns a temporary draw coordinate without assigning
# back into Player::positionCenter/prevPositionCenter.
present = PLAYER.split("ZunVec3 PresentRemotePlayer(Player *player, const ZunVec3 &target)", 1)[1].split(
    "u8 GetPlayerOverlapAlpha", 1
)[0]
assert "RemotePresentationState &state" in present
assert "SDL_GetTicksNS()" in present
assert "state.position = state.position.Lerp(target, blend);" in present
assert "12.0f * 12.0f" in present
assert "player->positionCenter =" not in present
assert "player->prevPositionCenter =" not in present

# The Player draw path owns the correction locally. Orb attachments and the
# optional visible hitbox receive the same corrected position, while the
# authoritative logical position remains untouched.
draw = PLAYER.split("ChainCallbackResult Player::OnDrawHighPrio(Player *p)", 1)[1].split(
    "ChainCallbackResult Player::OnDrawLowPrio", 1
)[0]
assert "logicalDrawPlayerPosition" in draw
assert "drawPlayerPosition = PresentRemotePlayer" in draw
assert "remoteDrawOffset = drawPlayerPosition - logicalDrawPlayerPosition" in draw
assert "p->orbsSprite[0].pos += remoteDrawOffset;" in draw
assert "p->orbsSprite[1].pos += remoteDrawOffset;" in draw
assert "DrawEaglerHitbox" in draw and "drawPlayerPosition.x" in draw and "drawPlayerPosition.y" in draw
assert "p->positionCenter = drawPlayerPosition" not in draw

# Alpha is presentation-only: clamp both actual TH06 interpolation endpoints,
# draw, then restore them. This prevents draw-time opacity from becoming future
# simulation/ANM state.
clamp = PLAYER.split("void ClampVmAlpha(AnmVm *vm, u8 alpha)", 1)[1].split(
    "u8 PlayerCharacter", 1
)[0]
assert "vm->color" in clamp
assert "vm->prevColor" in clamp
assert "originalPlayerColor" in draw and "originalPlayerPrevColor" in draw
assert "p->playerSprite.color = originalPlayerColor;" in draw
assert "p->playerSprite.prevColor = originalPlayerPrevColor;" in draw
assert "originalOrb0Color" in draw and "originalOrb0PrevColor" in draw
assert "originalOrb1Color" in draw and "originalOrb1PrevColor" in draw

# Presentation offsets must not leak into other gameplay owners.
all_sources = "\n".join(
    path.read_text(encoding="utf-8", errors="ignore")
    for path in (ROOT / "src").rglob("*.cpp")
)
assert all_sources.count("g_RemoteDrawOffsets") == PLAYER.count("g_RemoteDrawOffsets")

print("TH06 multiplayer presentation contract: PASS")
