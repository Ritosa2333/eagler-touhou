from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
game_manager = (ROOT / "src/GameManager.cpp").read_text(encoding="utf-8")
resources = (ROOT / "src/MultiplayerResources.cpp").read_text(encoding="utf-8")
enemy = (ROOT / "src/EnemyManager.cpp").read_text(encoding="utf-8")
player = (ROOT / "src/Player.cpp").read_text(encoding="utf-8")
player_h = (ROOT / "src/Player.hpp").read_text(encoding="utf-8")
items = (ROOT / "src/ItemManager.cpp").read_text(encoding="utf-8")
canonical = (ROOT / "src/netplay/Th06CanonicalHash.cpp").read_text(encoding="utf-8")
rollback = (ROOT / "src/netplay/Th06RollbackState.cpp").read_text(encoding="utf-8")
probe = (ROOT / "src/netplay/Th06LanStageProbe.cpp").read_text(encoding="utf-8")
gui = (ROOT / "src/Gui.cpp").read_text(encoding="utf-8")
ascii_manager = (ROOT / "src/AsciiManager.cpp").read_text(encoding="utf-8")


def require(ok: bool, label: str) -> None:
    if not ok:
        raise AssertionError(label)
    print(f"PASS: {label}")


require("GetActivePlayerCount()" in resources,
        "multiplayer balance reads stable active player count")
require("return 2.0f / 3.0f" in resources and "return 0.75f" in resources,
        "2P/3P boss damage scale is defined")
require("curEnemy->flags.isBoss" in enemy and "GetMultiplayerBossDamageMultiplier()" in enemy,
        "boss-only effective damage is scaled")
require("scoreDamage" in enemy and enemy.index("scoreDamage") < enemy.index("GetMultiplayerBossDamageMultiplier()"),
        "score accounting remains separate from boss durability scaling")
require("playerDamage[TH06_MULTI_MAX_PLAYERS]" in enemy and
        "static_cast<std::int64_t>(damage) * playerDamage[playerId]" in enemy and
        "AddPlayerDamageDealt(playerId" in enemy and
        "damage - damageAttributed" in enemy,
        "actual post-scaling damage is proportionally attributed with deterministic remainder")
require("playerDamage[playerId] > playerDamage[damageOwnerId]" in enemy and
        "AddPlayerEnemiesDefeated(damageOwnerId, 1)" in enemy,
        "defeat ownership uses highest damage and stable lower-slot tie breaking")
require("TouchObject(&g_MultiplayerContributionStats)" in rollback and
        "HashObject(hash, g_MultiplayerContributionStats)" in rollback,
        "rollback journal and local restore hash include contribution state")
require("for (const MultiplayerContributionStats &stats" in canonical and
        "hash.Scalar(stats.enemiesDefeated)" in canonical and
        "hash.Scalar(stats.damageDealt)" in canonical,
        "authoritative canonical hash includes every contribution counter")
require("GetMultiplayerRankPenalty(200)" in player and
        "GetMultiplayerRankPenalty(1600)" in player,
        "per-player bomb/death rank penalties are normalized")
require("for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)" in game_manager and
        "AddPlayerLives(playerId, 1)" in game_manager and
        "IsPlayerPermanentlyDeparted" in game_manager,
        "shared TH06 score extend reaches every active multiplayer participant")
require(game_manager.count("g_GameManager.IncreaseSubrank(200)") == 1,
        "team score extend applies original rank increase once")
require("PLAYER_STATE_REVIVABLE" in player_h and "PLAYER_STATE_SPIRIT" not in player_h,
        "TH06 uses its own multiplayer revivable state instead of importing TH07 Spirit")
require("p->playerState = PLAYER_STATE_REVIVABLE" in player and
        "if (!multiplayer)" in player and "g_GameManager.extraLives = -1" in player,
        "one multiplayer terminal death does not trigger TH06 single-player extend sentinel")
require("REVIVABLE_DRIFT_SPEED = 0.2f" in player and
        player.count("g_Rng.GetRandomU16() & 1") >= 2,
        "revivable drift follows TH07 Spirit's two synchronized RNG direction choices")
require("LIFE_GIVE_HOLD_FRAMES = 90" in player and
        "POWER_GIVE_TAPS_REQUIRED = 8" in player and
        "POWER_GIVE_TAP_WINDOW = 24" in player and
        "POWER_GIVE_AMOUNT = 20" in player,
        "TH06 life and Power transfer gestures use deterministic logical-frame rules")
require("index < 2 ? ITEM_POWER_BIG : ITEM_POWER_SMALL" in player and
        "g_ItemManager.CanSpawnItems(6)" in player,
        "Power transfer atomically converts 20 Power into two big and four small TH06 items")
require("ITEM_STATE_TRANSFER_P1 = 6" in items and
        "ITEM_STATE_TRANSFER_P3 = 8" in items and
        "ITEM_TRANSFER_RISE_FRAMES = 20" in items and
        "ITEM_TRANSFER_RISE_DISTANCE = 60.0f" in items and
        "1.0f - powf(1.0f - throwTime, 1.5f)" in items,
        "transfer items use TH07's 20-frame eased throw before TH06 fixed-target homing")
require("SelectLowestLifeRecipient" in player and
        "GetItemTransferStateForPlayer(static_cast<u8>(recipientId))" in player,
        "terminal players follow TH07 by transferring a Life item to the lowest-life survivor")
require("receiver->playerState = PLAYER_STATE_INVULNERABLE" in player and
        "receiver->orbState = ORB_UNFOCUSED" in player and
        "receiver->invulnerabilityTimer.SetCurrent(120)" in player and
        "receiver->bulletGracePeriod = 60" in player,
        "teammate revival follows TH07's direct invulnerable-state restoration")
require("PLAYER_STATE_REVIVABLE" in player[player.index("Player::CheckGraze"):],
        "revivable players are excluded from collision and graze paths")
assert "LIFE_GIVE_WAIT_RELEASE_TOKEN = -1" in player
assert "giver->lifeGiveTargetToken == LIFE_GIVE_WAIT_RELEASE_TOKEN" in player
assert "if (!giver->isFocus)" in player
assert player.count("giver->lifeGiveTargetToken = LIFE_GIVE_WAIT_RELEASE_TOKEN;") >= 2

for field in ("lifeGiveTimer", "lifeGiveTargetToken", "powerGiveTaps", "powerGiveWindow"):
    require(f"hash.Scalar(player.{field})" in canonical,
            f"canonical hash includes {field}")
require("TouchObject(&g_Players[playerId])" in rollback,
        "rollback captures the complete multiplayer Player including transfer timers")
require("IsPlayerTerminal(playerId)" in probe and "PLAYER_STATE_ELIMINATED" not in probe,
        "short team-wipe probe follows generic terminal state instead of old elimination-only logic")
require("compactMultiplayerHud" in gui and
        "const bool compactMultiplayerHud = MultiplayerGameplay::IsMultiplayer();" in gui and
        "GetPlayerLives(static_cast<u8>(playerId))" in gui and
        "GetPlayerBombs(static_cast<u8>(playerId))" in gui and
        "GetPlayerPower(static_cast<u8>(playerId))" in gui,
        "TH06 compact multiplayer HUD displays every player's independent resources")
require("MultiplayerGameplay::IsMultiplayer()" in ascii_manager and
        "g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;" in ascii_manager and
        ascii_manager.index("MultiplayerGameplay::IsMultiplayer()") <
        ascii_manager.index("if (g_GameManager.isInPracticeMode)"),
        "TH06 multiplayer hard-disables the ordinary Retry/Continue menu")
wipe_countdown = player.split("void UpdateTeamWipeRetryCountdownImpl()", 1)[1].split(
    "f32 PlayerSpawnX", 1
)[0]
require("g_GameManager.isInRetryMenu = 0;" in wipe_countdown and
        "g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;" in wipe_countdown and
        "g_GameManager.isInRetryMenu = 1;" not in wipe_countdown,
        "team wipe grace exits directly to Result without exposing Continue")
require('AddString(&textPos, "AWAY")' in gui and
        "GetMultiplayerHudLoadoutName" in gui and
        "playerLabelVm" in gui and "powerLabelVm" in gui and
        "resourceIconStep = 11.0f" in gui,
        "multiplayer HUD follows TH07's loadout, native-label, and compact-icon structure")
require("rowBackgroundVm = this->impl->vms[22]" in gui and
        "g_AnmManager->DrawNoRotation(&rowBackgroundVm)" in gui,
        "multiplayer HUD actively covers the original single-player resource rows")
require('"K:%u D:%u"' in gui and "ShouldShowContributionStats()" in gui,
        "optional multiplayer HUD exposes per-slot kill and damage contribution")
require("compactMultiplayerHud ? multiplayerHudBaseY + 148.0f : 206.0f" in gui and
        "compactMultiplayerHud ? multiplayerHudBaseY + 164.0f : 226.0f" in gui,
        "shared TH06 Graze/Point rows follow TH07's offsets below the multiplayer rows")
require("const bool sampledLogicalTouch = sampledReplayTouch || sampledNetplayTouch;" in player and
        "const bool consumeSynchronizedLocalTouch" in player and
        "sampledNetplayTouch && !speculative && canSampleRawTouch" in player and
        "this->initParam == MultiplayerGameplay::GetLocalPlayerSlot()" in player,
        "TH06 synchronized local direct touch follows TH07 ownership/consumption gating")
require(player.count("(!sampledLogicalTouch || consumeSynchronizedLocalTouch)") >= 2 and
        "Touch::ConsumePlayerDelta(consumeX, consumeY);" in player and
        "Touch::SetPlayerDelta(0.0f, 0.0f);" in player,
        "TH06 consumes the local raw touch delta after deterministic netplay replay of that sample")

print("TH06 multiplayer cooperative feature contract: PASS")
