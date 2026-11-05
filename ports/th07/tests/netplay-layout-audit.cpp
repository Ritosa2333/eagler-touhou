#include <cstddef>
#include <cstdio>

#include "BulletManager.hpp"
#include "EffectManager.hpp"
#include "EnemyManager.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Stage.hpp"
#include "EclManager.hpp"
#include "AsciiManager.hpp"

int main()
{
#define PRINT_OFFSET(type, field) std::printf(#type "." #field " %zu\n", offsetof(type, field))

    std::printf("Player %zu PlayerBullet %zu PlayerBombInfo %zu PlayerBombSubInfo %zu\n",
                sizeof(Player), sizeof(PlayerBullet), sizeof(PlayerBombInfo),
                sizeof(PlayerBombSubInfo));
    PRINT_OFFSET(Player, bullets);
    std::printf("Player.bullets bytes %zu count %zu\n",
                sizeof(((Player *)0)->bullets), sizeof(((Player *)0)->bullets) / sizeof(PlayerBullet));
    PRINT_OFFSET(Player, timers);
    PRINT_OFFSET(Player, bombInfo);
    PRINT_OFFSET(Player, bombStartPos);

    std::printf("EnemyManager %zu Enemy %zu\n", sizeof(EnemyManager), sizeof(Enemy));
    PRINT_OFFSET(EnemyManager, enemies);
    PRINT_OFFSET(EnemyManager, bosses);

    std::printf("BulletManager %zu Bullet %zu Laser %zu BulletTypeSprites %zu\n",
                sizeof(BulletManager), sizeof(Bullet), sizeof(Laser), sizeof(BulletTypeSprites));
    PRINT_OFFSET(BulletManager, bullets);
    PRINT_OFFSET(BulletManager, lasers);
    PRINT_OFFSET(BulletManager, bulletCount);

    std::printf("ItemManager %zu Item %zu\n", sizeof(ItemManager), sizeof(Item));
    PRINT_OFFSET(ItemManager, items);
    PRINT_OFFSET(ItemManager, nextIndex);
    PRINT_OFFSET(ItemManager, listHead);

    std::printf("EffectManager %zu Effect %zu\n", sizeof(EffectManager), sizeof(Effect));
    PRINT_OFFSET(EffectManager, effects);
    PRINT_OFFSET(EffectManager, layer0);
    PRINT_OFFSET(EffectManager, frameCounter);
    std::printf("GameManager %zu ZunGlobals %zu GameConfiguration %zu Stage %zu Gui %zu GuiImpl %zu EclGlobalVars %zu AsciiManager %zu\n",
                sizeof(GameManager), sizeof(ZunGlobals), sizeof(GameConfiguration), sizeof(Stage),
                sizeof(Gui), sizeof(GuiImpl), sizeof(EclGlobalVars), sizeof(AsciiManager));
}
