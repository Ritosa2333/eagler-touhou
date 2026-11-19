#pragma once
#include "BulletManager.hpp"
#include "BulletVisuals.hpp"
#include <limits>

namespace th128::bullet {
using GrazeCallback=std::function<bool(const Birth&,std::string&)>;
using HitCallback=std::function<std::optional<int>(const Birth&,std::string&)>;
using CancellationCallback=std::function<bool(const Birth&,bool,std::string&)>;
using ExtensionSound=std::function<bool(int,std::string&)>;
using EnemySpawnCallback=std::function<bool(const Birth&,const ExtensionRaw&,int,std::string&)>;
using RestartCallback=std::function<bool(Birth&,BulletVisuals&,BulletVisuals::Visual&,const ExtensionRaw&,int,std::string&)>;
bool updateBulletWithKnownMisses(Birth& bullet,BulletVisuals& visuals,BulletVisuals::Visual& visual,
                                 Position player,float profileHitRadius,bool& removed,std::string& error,
                                 const GrazeCallback& graze={},const HitCallback& hit={},const CancellationCallback& cancellation={},const ExtensionSound& sound={},
                                 const Manager::NestedEmitter& nestedEmitter={},
                                 const std::function<const Profile*(int)>& profileForType={},
                                 const RestartCallback& restart={},
                                 const EnemySpawnCallback& spawnEnemy={},const std::optional<Position>& playerPathStart={});
}
