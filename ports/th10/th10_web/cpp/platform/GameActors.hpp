#pragma once
#include "../game/GameSession.hpp"
#include "../game/SpellCard.hpp"
#include "../game/Results.hpp"
#include "../game/ScorePopups.hpp"
#include "../game/StageHints.hpp"
#include "../game/GameObjectResources.hpp"
#include "../game/LaserManager.hpp"
namespace th10::browser {
// Typed scene references. Owners update these at their creation/destruction
// boundaries; environments resolve the current object at each game callback.
struct GameActors {
    GameSession* session=nullptr;Player* player=nullptr;EnemyManager* enemies=nullptr;
    EnemyBulletManager* bullets=nullptr;ItemManager* items=nullptr;LaserManager* lasers=nullptr;
    Gui* gui=nullptr;Results* results=nullptr;ScorePopups* popups=nullptr;
    StageHints* hints=nullptr;GameEffects* effects=nullptr;Bomb* bomb=nullptr;SpellCard* spell=nullptr;
};
}
