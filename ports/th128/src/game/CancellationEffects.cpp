#include "CancellationEffects.hpp"
#include <unordered_set>

namespace th128::effects {
namespace {

struct ExecutionGuard {
    bool& executing;
    explicit ExecutionGuard(bool& value) : executing(value) { executing = true; }
    ~ExecutionGuard() { executing = false; }
};

}

bool CancellationEffects::validatePrograms(const anm::Registry& registry, std::unordered_map<int, size_t>& reservations,
                                           std::string& error) {
    const auto bank = registry.bank(BankId);
    if (!bank || bank->scripts.size() <= size_t(std::max(CancelScript, ChildScript))) {
        error = "unified effect bank requires actual scripts83/84/175/357/356";
        return false;
    }
    size_t childCount = 0;
    bool rootBranches = false;
    const bool deathPrograms=bank->scripts[88].commands.size()>1&&std::any_of(bank->scripts[88].commands.begin(),bank->scripts[88].commands.end(),[](const auto& command){return command.op==88;});
    const bool largeCancel=bank->scripts.size()>375&&std::any_of(bank->scripts[375].commands.begin(),bank->scripts[375].commands.end(),[](const auto& command){return command.op==88;});
    size_t largeChildren=0;
    const bool ovalCancel=bank->scripts.size()>381&&std::any_of(bank->scripts[381].commands.begin(),bank->scripts[381].commands.end(),[](const auto& command){return command.op==88;});
    size_t ovalChildren=0;
    const bool coloredPrograms=bank->scripts.size()>367&&std::any_of(bank->scripts[337].commands.begin(),bank->scripts[337].commands.end(),[](const auto& command){return command.op==88;});
    const bool type25Cancel=bank->scripts.size()>371&&std::any_of(bank->scripts[371].commands.begin(),bank->scripts[371].commands.end(),[](const auto& command){return command.op==88;});
    const bool rainbowCancel=bank->scripts.size()>377&&std::any_of(bank->scripts[377].commands.begin(),bank->scripts[377].commands.end(),[](const auto& command){return command.op==88;});
    const bool sibling369Cancel=bank->scripts.size()>369&&std::any_of(bank->scripts[369].commands.begin(),bank->scripts[369].commands.end(),[](const auto& command){return command.op==88;});
    const bool sibling373Cancel=bank->scripts.size()>373&&std::any_of(bank->scripts[373].commands.begin(),bank->scripts[373].commands.end(),[](const auto& command){return command.op==88;});
    const bool sibling379Cancel=bank->scripts.size()>379&&std::any_of(bank->scripts[379].commands.begin(),bank->scripts[379].commands.end(),[](const auto& command){return command.op==88;});
    const bool sibling383Cancel=bank->scripts.size()>383&&std::any_of(bank->scripts[383].commands.begin(),bank->scripts[383].commands.end(),[](const auto& command){return command.op==88;});
    size_t type25Children=0;
    size_t rainbowChildren=0;
    size_t sibling369Children=0;
    size_t sibling373Children=0;
    size_t sibling379Children=0;
    size_t sibling383Children=0;
    std::unordered_map<int,size_t> coloredChildren;
    std::unordered_map<int,size_t> deathChildren;
    auto reject = [&error](const std::string& reason) { error = "cancellation " + reason; return false; };
    std::vector<int> scripts{83,84,175,CancelScript,ChildScript};
    if(deathPrograms)for(int script=85;script<=97;++script)scripts.push_back(script);
    if(largeCancel){scripts.push_back(374);scripts.push_back(375);}
    if(ovalCancel){scripts.push_back(380);scripts.push_back(381);}
    if(coloredPrograms)for(int script=336;script<=367;++script)if(script!=CancelScript&&script!=ChildScript)scripts.push_back(script);
    if(type25Cancel){scripts.push_back(370);scripts.push_back(371);}
    if(rainbowCancel){scripts.push_back(376);scripts.push_back(377);}
    if(sibling369Cancel){scripts.push_back(368);scripts.push_back(369);}
    if(sibling373Cancel){scripts.push_back(372);scripts.push_back(373);}
    if(sibling379Cancel){scripts.push_back(378);scripts.push_back(379);}
    if(sibling383Cancel){scripts.push_back(382);scripts.push_back(383);}
    for (const int script : scripts) {
        for (const auto& command : bank->scripts[script].commands) {
            const int opcode = command.op;
            if (opcode == -1)
                continue;
            if (opcode == 90 || opcode == 91 || opcode == 92 || opcode == 95 || opcode == 96 || opcode == 97)
                return reject("unproved child callback opcode=" + std::to_string(opcode));
            if (!((opcode >= 0 && opcode <= 43) || (opcode >= 48 && opcode <= 63) || opcode == 66 ||
                  opcode == 67 || opcode == 68 || opcode == 75 || opcode == 87 || opcode == 88 || opcode == 111))
                return reject("unsupported opcode=" + std::to_string(opcode));
            for (size_t operand = 0; operand < command.words.size() && operand < 16; ++operand)
                if (command.mask & (1u << operand)) {
                    const int builtin = anm::nativeInt(anm::asFloat(command.words[operand]));
                    if (builtin >= 10016 && builtin <= 10021)
                        return reject("unproved camera operand");
                }
            if ((opcode >= 6 && opcode <= 27) || opcode == 5 || (opcode >= 40 && opcode <= 43)) {
                const bool floating = opcode == 42 || opcode == 43 || (opcode != 5 && (opcode & 1));
                const int destination = floating ? anm::nativeInt(anm::asFloat(command.words[0]))
                                                 : int32_t(command.words[0]);
                const bool variable = floating
                    ? (destination >= 10004 && destination <= 10007) || (destination >= 10013 && destination <= 10015) ||
                      (destination >= 10023 && destination <= 10025)
                    : (destination >= 10000 && destination <= 10003) || destination == 10008 || destination == 10009;
                if (!(command.mask & 1) || !variable)
                    return reject("unproved script operand mutation");
            }
            if (opcode == 3 && (command.mask || int32_t(command.words[0]) < 0 || command.words[0] >= bank->sprites.size()))
                return reject("unsupported sprite selection");
            if (opcode == 51 || opcode == 52) {
                if (command.mask)
                    return reject("unproved variable color");
                for (const auto color : command.words)
                    if (int32_t(color) < 0 || color > 255)
                        return reject("invalid color component");
            }
            if (opcode >= 56 && opcode <= 60 && (int32_t(command.words[1]) < 0 || int32_t(command.words[1]) > 6))
                return reject("unproved interpolation mode");
            if (opcode == 67 && (command.mask || command.words[0] > 1))
                return reject("unsupported geometry mode");
            if (opcode == 66 && (command.mask || command.words[0] > 1))
                return reject("unsupported geometry blend");
            if (opcode == 68 && (command.mask || command.words[0] > 30))
                return reject("invalid draw layer");
            if (opcode == 87 && (command.mask || command.words[0] != 0))
                return reject("display RNG domain is unconfigured");
            if (opcode == 111 && (command.mask || command.words[0] != 0))
                return reject("opcode111 external callback is unproved");
            if (opcode == 88) {
                if(script>=85&&script<=94&&(script-85)%3==0){
                    const auto target=int(command.words[0]);
                    if(command.mask||(target!=script+1&&target!=script+2&&target!=97)||++deathChildren[script]>10)
                        return reject("unproved death child program");
                }else if(script==375){
                    if(command.mask||command.words[0]!=374||++largeChildren>3)return reject("unproved type18 cancellation children");
                }else if(script==381){
                    if(command.mask||command.words[0]!=380||++ovalChildren>3)return reject("unproved type20 cancellation children");
                }else if(script==371){
                    if(command.mask||command.words[0]!=370||++type25Children>3)return reject("unproved type25 cancellation children");
                }else if(script==377){
                    if(command.mask||command.words[0]!=376||++rainbowChildren>3)return reject("unproved rainbow cancellation children");
                }else if(script==369){
                    if(command.mask||command.words[0]!=368||++sibling369Children>3)return reject("unproved sibling369 cancellation children");
                }else if(script==373){
                    if(command.mask||command.words[0]!=372||++sibling373Children>3)return reject("unproved sibling373 cancellation children");
                }else if(script==379){
                    if(command.mask||command.words[0]!=378||++sibling379Children>3)return reject("unproved sibling379 cancellation children");
                }else if(script==383){
                    if(command.mask||command.words[0]!=382||++sibling383Children>3)return reject("unproved sibling383 cancellation children");
                }else if(script==370){
                    return reject("unproved type25 cancellation child-of-child");
                }else if(coloredPrograms&&script>=337&&script<=367&&(script&1)){
                    if(command.mask||command.words[0]!=std::uint32_t(script-1)||++coloredChildren[script]>2)return reject("unproved color cancellation children");
                }else if(script!=CancelScript||command.mask||command.words[0]!=ChildScript||++childCount>2)
                    return reject("unproved child program or child-of-child creation");
            }
            if(script>=85&&script<=97&&opcode==4)return reject("unproved death program loop");
            if((script==374||script==375||script==380||script==381)&&(opcode==4||opcode==5||(opcode>=28&&opcode<=39)))return reject("unproved large cancellation loop");
            if((script==368||script==369||script==372||script==373||script==378||script==379||script==382||script==383)&&(opcode==4||opcode==5||(opcode>=28&&opcode<=39)))return reject("unproved sibling cancellation loop");
            if(coloredPrograms&&script>=336&&script<=367&&(opcode==4||opcode==5||(opcode>=28&&opcode<=39)))return reject("unproved color cancellation loop");
            if((script==370||script==371)&&(opcode==4||opcode==5||(opcode>=28&&opcode<=39)))return reject("unproved type25 cancellation loop");
            if (script == CancelScript && (opcode == 4 || opcode == 5 || (opcode >= 28 && opcode <= 39)))
                rootBranches = true;
        }
    }
    if (childCount && rootBranches)
        return reject("repeating child creation is unproved");
    reservations = {{83, 1}, {84, 1}, {175, 1}, {CancelScript, 1 + childCount}};
    if(deathPrograms)for(const int script:{85,88,91,94})reservations[script]=1+deathChildren[script];
    if(largeCancel)reservations[375]=1+largeChildren;
    if(ovalCancel)reservations[381]=1+ovalChildren;
    if(coloredPrograms)for(int script=337;script<=367;script+=2)reservations[script]=1+coloredChildren[script];
    if(coloredPrograms)reservations[CancelScript]=1+coloredChildren[CancelScript];
    if(type25Cancel)reservations[371]=1+type25Children;
    if(rainbowCancel)reservations[377]=1+rainbowChildren;
    if(sibling369Cancel)reservations[369]=1+sibling369Children;
    if(sibling373Cancel)reservations[373]=1+sibling373Children;
    if(sibling379Cancel)reservations[379]=1+sibling379Children;
    if(sibling383Cancel)reservations[383]=1+sibling383Children;
    return true;
}

bool CancellationEffects::load(const void* bytes, size_t size, std::string& error) {
    error.clear();
    if (executing_) {
        error = "cancellation callback reentry is unsupported";
        return false;
    }
    anm::Scene candidate;
    std::unordered_map<int, size_t> reservations;
    if (!candidate.registry.loadBank(BankId, static_cast<const uint8_t*>(bytes), size, error) ||
        !validatePrograms(candidate.registry, reservations, error))
        return false;
    scene_.reset();
    scene_.registry = std::move(candidate.registry);
    roots_.clear();
    reservedNodes_ = 0;
    nodesPerScript_ = std::move(reservations);
    failure_.clear();
    configureRuntime();
    return true;
}

bool CancellationEffects::setGameRandom(std::function<uint32_t()> random) {
    if (executing_)
        return false;
    gameRandom32_ = std::move(random);
    return true;
}

uint32_t CancellationEffects::random32() {
    if (!failure_.empty())
        return 0;
#if defined(__cpp_exceptions)
    try {
        return gameRandom32_();
    } catch (...) {
        failure_ = "cancellation game RNG callback threw";
        return 0;
    }
#else
    return gameRandom32_();
#endif
}

void CancellationEffects::configureRuntime() {
    scene_.runtime = {};
    scene_.runtime.instructionBudget = InstructionBudget;
    if (gameRandom32_)
        scene_.runtime.gameRandom32 = [this] { return random32(); };
    scene_.runtime.unsupported = [this](int bank, int script, int opcode, const char* reason) {
        if (failure_.empty())
            failure_ = "cancellation ANM bank=" + std::to_string(bank) + " script=" + std::to_string(script) +
                       " opcode=" + std::to_string(opcode) + ": " + reason;
    };
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
}

bool CancellationEffects::ready(std::string& error) const {
    error.clear();
    if (executing_) {
        error = "cancellation callback reentry is unsupported";
        return false;
    }
    if (!failure_.empty()) {
        error = failure_;
        return false;
    }
    return true;
}

bool CancellationEffects::fail(std::string& error) {
    if (failure_.empty())
        failure_ = "cancellation Scene execution failed";
    error = failure_;
    scene_.reset();
    roots_.clear();
    reservedNodes_ = 0;
    unvalidatedMutation_ = false;
    return false;
}

bool CancellationEffects::validState(const anm::State& state) {
    if (state.mode < 0 || state.mode > 1 || state.layer < 0 || state.layer > 30)
        return false;
    for (const float value : {state.x, state.y, state.z, state.engineX, state.engineY, state.engineZ,
                             state.offsetX, state.offsetY, state.offsetZ, state.rx, state.ry, state.rotation,
                             state.sx, state.sy, state.u, state.v, state.uvScaleX, state.uvScaleY,
                             state.spriteWidth, state.spriteHeight})
        if (!std::isfinite(value))
            return false;
    for (const auto value : state.floats)
        if (!std::isfinite(value))
            return false;
    return true;
}

bool CancellationEffects::worldPose(const anm::Node& node, Pose& pose, size_t depth) const {
    if (depth >= 64)
        return false;
    const auto& state = node.vm.state();
    pose = {state.x + state.engineX + state.offsetX, state.y + state.engineY + state.offsetY,
            state.z + state.engineZ + state.offsetZ, state.sx, state.sy, state.rotation};
    if (node.parent) {
        const auto parent = scene_.nodes().find(node.parent);
        Pose inherited;
        if (parent == scene_.nodes().end() || !parent->second->alive || parent->second->owner != node.owner ||
            !worldPose(*parent->second, inherited, depth + 1))
            return false;
        pose.x += inherited.x;
        pose.y += inherited.y;
        pose.z += inherited.z;
        pose.sx *= inherited.sx;
        pose.sy *= inherited.sy;
        if (state.mode == 1)
            pose.rotation += inherited.rotation;
    }
    return std::isfinite(pose.x) && std::isfinite(pose.y) && std::isfinite(pose.z) &&
           std::isfinite(pose.sx) && std::isfinite(pose.sy) && std::isfinite(pose.rotation);
}

bool CancellationEffects::reconcileSpawned(Handle handle, size_t beforeNodes, bool& accepted) {
    // Accepts only when this bind created exactly the root plus its physical
    // childNext successors, all still owned and valid, and the reservation still
    // covers them. Anything unproven returns accepted=false so the caller runs
    // the unmodified full reconcile (same diagnostics, same failure text).
    accepted = false;
    const auto owned = roots_.find(handle);
    const auto* root = scene_.find(handle);
    if (owned == roots_.end() || !root || !root->alive || root->parent != 0)
        return true;
    const size_t afterNodes = scene_.nodes().size();
    if (afterNodes < beforeNodes || afterNodes > reservedNodes_)
        return true;
    const size_t delta = afterNodes - beforeNodes;
    if (delta == 0)
        return true;
    std::unordered_set<uint32_t> nodes;
    nodes.insert(root->id);
    for (const auto* child : scene_.children(root->id))
        if (child) nodes.insert(child->id);
    if (nodes.size() != delta)
        return true;
    for (const auto id : nodes) {
        const auto at = scene_.nodes().find(id);
        if (at == scene_.nodes().end())
            return true;
        const auto& node = *at->second;
        if (!node.alive || node.owner != handle || node.detached || node.customGeometry ||
            node.spriteRemap || node.beforeTick || node.onInterrupt)
            return true;
        const auto& state = node.vm.state();
        if (node.vm.faulted() || !validState(state))
            return true;
        if ((state.visible && state.sprite < 0) ||
            (state.sprite >= 0 && (state.spriteBank != BankId || !spriteInfo(state.sprite))))
            return true;
        if (state.ended || state.deleteRequested)
            return true;
        if (node.parent != 0) {
            if (!nodes.contains(node.parent))
                return true;
            const auto parent = scene_.nodes().find(node.parent);
            if (parent == scene_.nodes().end() || !parent->second->alive ||
                parent->second->owner != handle)
                return true;
        }
        Pose pose;
        if (!worldPose(node, pose))
            return true;
    }
    const size_t newNodes = nodes.size();
    if (root->vm.state().script == 355) {
        const auto bank = scene_.registry.bank(BankId);
        if (!bank)
            return true;
        const auto diagnostic = root->vm.diagnostics();
        if (diagnostic.pc > bank->scripts[355].commands.size() || diagnostic.pendingInterrupt ||
            diagnostic.hasSavedTimer)
            return true;
        size_t required = newNodes;
        for (size_t command = diagnostic.pc; command < bank->scripts[355].commands.size(); ++command) {
            const auto& entry = bank->scripts[355].commands[command];
            if (entry.op != 88) continue;
            if (entry.mask != 0 || entry.words.size() != 1 || entry.words[0] != 354u)
                return true;
            ++required;
        }
        if (!required || required > owned->second.reservation)
            return true;
        reservedNodes_ -= owned->second.reservation - required;
        owned->second.reservation = required;
    } else if (newNodes > owned->second.reservation) {
        return true;
    }
    unvalidatedMutation_ = false;
    accepted = true;
    return true;
}

bool CancellationEffects::reconcile(std::string& error) {
    if (!failure_.empty())
        return fail(error);
    if (scene_.nodes().size() > reservedNodes_) {
        failure_ = "cancellation Scene node/reservation budget exceeded";
        return fail(error);
    }
    unvalidatedMutation_ = false;
    for (const auto& [id, node] : scene_.nodes()) {
        if (node->vm.faulted() || node->detached || node->customGeometry || node->spriteRemap ||
            node->beforeTick || node->onInterrupt || !roots_.contains(node->owner) || !validState(node->vm.state())) {
            failure_ = "cancellation unsupported Scene state/geometry/callback";
            return fail(error);
        }
        const auto& state = node->vm.state();
        if ((state.visible && state.sprite < 0) ||
            (state.sprite >= 0 && (state.spriteBank != BankId || !spriteInfo(state.sprite)))) {
            failure_ = "cancellation unsupported sprite resource";
            return fail(error);
        }
        if (state.ended || state.deleteRequested)
            node->alive = false;
    }
    scene_.collect();
    for (auto root = roots_.begin(); root != roots_.end();) {
        if (!scene_.find(root->first)) {
            reservedNodes_ -= root->second.reservation;
            root = roots_.erase(root);
        } else
            ++root;
    }
    std::unordered_map<Handle,size_t> liveOwnedNodes;
    for(const auto& owned:scene_.nodes())++liveOwnedNodes[owned.second->owner];
    for(auto& owned:roots_){
        const auto* root=scene_.find(owned.first);
        if(!root||root->vm.state().script!=355)continue;
        const auto bank=scene_.registry.bank(BankId);
        const auto diagnostic=root->vm.diagnostics();
        const auto& commands=bank->scripts[355].commands;
        if(diagnostic.pc>commands.size()||diagnostic.pendingInterrupt||diagnostic.hasSavedTimer){
            failure_="cancellation355 remaining-child program state is unproved";return fail(error);
        }
        size_t pendingChildren=0;
        for(size_t commandIndex=diagnostic.pc;commandIndex<commands.size();++commandIndex){
            const auto& command=commands[commandIndex];
            if(command.op!=88)continue;
            if(command.mask||command.words.size()!=1||command.words[0]!=354u){
                failure_="cancellation355 remaining-child ownership is unproved";return fail(error);
            }
            ++pendingChildren;
        }
        const size_t required=liveOwnedNodes[owned.first]+pendingChildren;
        if(!required||required>owned.second.reservation){
            failure_="cancellation355 live/pending reservation increased unexpectedly";return fail(error);
        }
        reservedNodes_-=owned.second.reservation-required;
        owned.second.reservation=required;
    }
    for (const auto& [id, node] : scene_.nodes()) {
        Pose pose;
        if (!worldPose(*node, pose)) {
            failure_ = "cancellation invalid geometry ancestry/world pose";
            return fail(error);
        }
    }
    return true;
}

std::optional<CancellationEffects::Handle> CancellationEffects::spawn(int script, float x, float y, float z,
                                                                     int layer, std::string& error,bool head) {
    if (!ready(error))
        return std::nullopt;
    const auto reservation = nodesPerScript_.find(script);
    if (reservation == nodesPerScript_.end() || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) ||
        layer < 0 || layer > 30) {
        error = "unsupported cancellation script="+std::to_string(script)+" x="+std::to_string(x)+" y="+std::to_string(y)
+            " z="+std::to_string(z)+" layer="+std::to_string(layer);
        return std::nullopt;
    }
    if (!scene_.registry.bank(BankId) || !gameRandom32_) {
        error = "cancellation bank and shared game RNG callback are required";
        return std::nullopt;
    }
    if (!nextHandle_ || reservation->second > std::numeric_limits<size_t>::max() - reservedNodes_) {
        size_t rootReservationSum=0;
        for(const auto& owned:roots_)rootReservationSum+=owned.second.reservation;
        error = "cancellation handle/reservation arithmetic exhausted script="+std::to_string(script)
            +" sceneNodes="+std::to_string(scene_.nodes().size())+" roots="+std::to_string(roots_.size())
            +" reserved="+std::to_string(reservedNodes_)+" rootReservationSum="+std::to_string(rootReservationSum)
            +" incomingReservation="+std::to_string(reservation->second)+" nextHandle="+std::to_string(nextHandle_)
            +" maximum="+std::to_string(MaxNodes);
        std::unordered_map<Handle,size_t> liveByOwner;
        for(const auto& node:scene_.nodes())++liveByOwner[node.second->owner];
        std::unordered_map<int,std::array<size_t,6>> scriptCounts;
        const auto bank=scene_.registry.bank(BankId);
        for(const auto& owned:roots_){
            const auto* root=scene_.find(owned.first);
            const int rootScript=root?root->vm.state().script:-1;
            auto& counts=scriptCounts[rootScript];
            ++counts[0];counts[1]+=owned.second.reservation;counts[2]+=liveByOwner[owned.first];
            if(!root||!bank||rootScript<0||size_t(rootScript)>=bank->scripts.size()){++counts[5];continue;}
            const auto diagnostic=root->vm.diagnostics();
            const auto& commands=bank->scripts[size_t(rootScript)].commands;
            if(diagnostic.pc>commands.size()||diagnostic.pendingInterrupt||diagnostic.hasSavedTimer)++counts[5];
            else for(size_t command=diagnostic.pc;command<commands.size();++command)if(commands[command].op==88)++counts[3];
            counts[4]+=size_t(std::max(0,root->vm.state().ticks));
        }
        std::vector<int> scripts;
        for(const auto& counts:scriptCounts)scripts.push_back(counts.first);
        std::sort(scripts.begin(),scripts.end());
        for(const int selected:scripts){const auto& counts=scriptCounts[selected];
            error+=" scriptmix["+std::to_string(selected)+"]="+std::to_string(counts[0])+","
                +std::to_string(counts[1])+","+std::to_string(counts[2])+","+std::to_string(counts[3])+","
                +std::to_string(counts[4])+","+std::to_string(counts[5]);
        }
        return std::nullopt;
    }
    const Handle handle = nextHandle_++;
    roots_.emplace(handle, Root{reservation->second});
    reservedNodes_ += reservation->second;
    configureRuntime();
    ExecutionGuard guard(executing_);
    const size_t beforeNodes = scene_.nodes().size();
    const auto* node = scene_.bind(handle, handle, BankId, script, anm::Membership::Primary,
                                   x, y, {}, head, nullptr, 0, 0, layer, false, z);
    if (!node && failure_.empty())
        failure_ = "cancellation Scene root binding failed";
    if (failure_.empty() && !unvalidatedMutation_) {
        bool accepted = false;
        reconcileSpawned(handle, beforeNodes, accepted);
        if (accepted)
            return handle;
    }
    if (!reconcile(error))
        return std::nullopt;
    return handle;
}

bool CancellationEffects::phase(std::string& error) {
    if (!ready(error))
        return false;
    if (!scene_.nodes().empty() && !gameRandom32_) {
        error = "cancellation shared game RNG callback is required";
        return false;
    }
    configureRuntime();
    ExecutionGuard guard(executing_);
    scene_.tickPrimary();
    return reconcile(error);
}

bool CancellationEffects::tick(std::string& error) { return phase(error); }
bool CancellationEffects::tickPrimary(std::string& error) { return tick(error); }

bool CancellationEffects::setRootPositionInterpolation(Handle handle, float x, float y, float z,
                                                       int duration, int mode, std::string& error) {
    if (!ready(error))
        return false;
    auto* node = scene_.find(handle);
    if (!roots_.contains(handle) || !node || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) ||
        duration < 0 || duration > 4096 || mode < 0 || mode > 6) {
        error = "invalid cancellation handle/interpolation target/duration/mode";
        return false;
    }
    node->vm.interpolatePosition(x, y, z, duration, mode);
    unvalidatedMutation_ = true;
    return true;
}

bool CancellationEffects::initializeNativeDrift(Handle handle, const std::array<float, 3>& velocity, float clock,
                                                const std::array<float, 3>& globalStart, std::string& error) {
    if (!ready(error))
        return false;
    auto* node = scene_.find(handle);
    const int script=node?node->vm.state().script:-1;
    const bool colored=script>=337&&script<=367&&(script&1);
    if (!roots_.contains(handle) || !node || (!colored&&script!=CancelScript&&script!=375&&script!=381&&script!=371&&script!=377&&script!=369&&script!=373&&script!=379&&script!=383) ||
        clock != 1.f || node->vm.state().ticks != 1 ||
        anm::asWord(node->vm.state().x) || anm::asWord(node->vm.state().y) || anm::asWord(node->vm.state().z) ||
        !node->vm.diagnostics().interpolations.empty() ||
        !std::all_of(globalStart.begin(), globalStart.end(), [](float value) { return anm::asWord(value) == 0; })) {
        error = "unsupported native cancellation drift context: explicit positive-zero start, clock1 and fresh zero-script root required";
        return false;
    }
    std::array<float, 3> target{};
    for (size_t axis = 0; axis < target.size(); ++axis) {
        if (!std::isfinite(velocity[axis])) {
            error = "native cancellation drift velocity must be finite";
            return false;
        }
        const float stepped = float(velocity[axis] * clock);
        target[axis] = float(double(stepped) * 10.0);
        if (!std::isfinite(target[axis])) {
            error = "native cancellation drift target overflow";
            return false;
        }
    }
    node->vm.interpolatePosition(target[0], target[1], target[2], 30, 6);
    unvalidatedMutation_ = true;
    return true;
}

bool CancellationEffects::requestNativeDrift(Handle handle, std::string& error) {
    if (!ready(error))
        return false;
    if (!roots_.contains(handle) || !scene_.find(handle))
        error = "invalid cancellation handle";
    else
        error = "unhandled generic native cancellation drift; explicit clock1/zero-start initializer is required";
    return false;
}

bool CancellationEffects::remove(Handle handle) {
    if (executing_ || !roots_.contains(handle))
        return false;
    scene_.removeOwner(handle);
    std::string error;
    return reconcile(error);
}

bool CancellationEffects::clear() {
    if (executing_)
        return false;
    scene_.reset();
    roots_.clear();
    reservedNodes_ = 0;
    failure_.clear();
    unvalidatedMutation_ = false;
    return true;
}

std::vector<CancellationEffects::Snapshot> CancellationEffects::snapshots() const {
    std::vector<Snapshot> result;
    if (executing_ || !failure_.empty())
        return result;
    result.reserve(scene_.nodes().size());
    for (const auto& [id, node] : scene_.nodes()) {
        Pose pose;
        if (!node->alive || !worldPose(*node, pose))
            continue;
        const auto& state = node->vm.state();
        result.push_back({node->owner, id, node->parent, node->membership, state, spriteInfo(state.sprite), pose});
    }
    std::sort(result.begin(), result.end(), [](const Snapshot& first, const Snapshot& second) {
        return first.node < second.node;
    });
    return result;
}

std::optional<anm::SpriteInfo> CancellationEffects::spriteInfo(int sprite) const {
    const auto bank = scene_.registry.bank(BankId);
    if (!bank || sprite < 0 || size_t(sprite) >= bank->sprites.size())
        return std::nullopt;
    return bank->sprites[sprite];
}

}
