// scene99仅持既有生产上下文引用；原清零范围的未命名尾段独立保存。
#pragma once
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/ItemPool.hpp"
#include "core/player/PlayerBomb.hpp"
#include "core/player/PlayerShotPools.hpp"
#include "core/player/PlayerLifecycle.hpp"
#include "core/shared/DialogueState.hpp"
#include "core/shared/DialogueContext.hpp"
#include "core/stages/StageResources.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/stages/Stage6RuntimeContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/shared/MusicPresentationContext.hpp"
#include "core/stages/StageTitleContext.hpp"
#include "core/frontend/MenuStartContext.hpp"
#include "core/runtime/GameplayPresentationDispatchContext.hpp"
#include "core/frontend/ReplayPauseEntryContext.hpp"
#include <vector>
namespace kaikeidou {
struct GameStartEntryGaps {
 ObjectRecord<56> resource_reals_tail; // A73468..A7349F。
 ObjectRecord<24> resource_integers_tail; // A75840..A75857。
 ObjectRecord<64> pattern_reals_tail; // A87EB0..A87EEF；随后16字节直接别名enemy_prefix。
 std::uint32_t boss_a7ea4c=0,boss_a7ea54=0,boss_a7eb44=0;
 ObjectRecord<52> boss_a7ea64; // A7EA64..A7EA97。
 ObjectRecord<80> boss_a7eba8; // A7EBA8..A7EBF7；下一地址为SaveData.options。
};
struct GameStartEntryReplayContext {
 std::uint32_t buffer_address=0; // B43100。
 std::uint32_t mode=0,cursor=0; // B4310C、B43110。
 std::uint32_t metadata_mode=0; // B43255，原未对齐dword。
 std::array<std::uint8_t,247> filename_bytes{}; // B43288..B4337E，固定路径原字节。
 std::array<std::uint8_t,16> file_table_tail{}; // B433B3..B433C2，表后保留字节。
 std::array<std::uint32_t,13> file_names{}; // B4337F，原未对齐指针表。
 std::array<std::uint32_t,7> destinations{}; // B43800..B43818。
 std::uint32_t bytes_read=0,metadata_index=0,metadata_bytes_read=0,file_selected=0; // B43900/04/0C/40。
 std::uint32_t accumulated_score=0,practice_record=0,count=0; // B43A00/04/08。
 std::uint32_t checksum_word=0,checksum_sum=0,name_second=0; // B43C08/0C/14。
 // B43C04在本闭包未读写，仍由宿主既有Replay状态持有。
 struct Allocation {std::uint32_t address=0;std::vector<std::uint8_t> bytes;};
 std::vector<Allocation> allocations;
};
struct GameStartEntryRefs {
 ObjectRecord<240>& player;EnemyPool& enemies;ProjectilePools& projectiles;ItemPool& items;
 EffectPool& effects;EffectShake& shake;OptionsPool& options;OptionsContext& options_context;
 PlayerShotPools& shots;PlayerBombPool& bombs;PlayerShotFireContext& fire;
 DialogueState& dialogue;DialogueContext& dialogue_context;LifecycleContext& lifecycle;
 PatternParameters& patterns;EnemyDeathContext& death;StagePatternContext& stage_patterns;
 Stage3BossTimelineContext& extended_patterns;Stage4PatternContext& stage4_patterns;
 BossAllocationContext& boss_allocation;BossPhaseContext& boss_phase;BossPresentationContext& boss_presentation;
 BossStageContext& boss_stage;EnemyDamageContext& protection;Stage6RuntimeContext& runtime;
 MusicPresentationContext& music;StageTitleContext& title;StageResourceContext& resources;BackgroundMeshPool& meshes;
 SaveData& savedata;Random& random;MenuStartContext& menu;
 GameplayPresentationDispatchContext& gameplay;ReplayPauseEntryContext& pause;PlayerBombContext& bomb_context;
 std::uint32_t& practice_selected; // A87F30，MenuCharacterPracticeContext.practice_selected。
 std::uint32_t& weapon; // A7E614，MenuCharacterPracticeContext.weapon。
};
struct GameStartEntryRegisters {
 std::uint32_t eax=0,ecx=0,edx=0,ebx=0,esp=0,ebp=0,esi=0,edi=0,eflags=0x202;
};
struct GameStartEntryMachineFrame {
 GameStartEntryRegisters registers;
 std::uint32_t stack_address=0;
 std::vector<std::uint8_t> stack;
};
enum class GameStartEntryOutcomeKind {scene100,original_failure_transfer,memory_fault};
struct GameStartEntryOutcome {
 GameStartEntryOutcomeKind kind=GameStartEntryOutcomeKind::scene100;
 std::uint32_t target=0x65345a,fault_address=0,fault_bytes=0;
 bool write_fault=false;
 GameStartEntryRegisters registers;
 std::vector<std::uint32_t> open_handles;
};
}
