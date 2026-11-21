#include "EnemyVisuals.hpp"
#include "AppearanceBounds.hpp"

namespace th128::enemy {
namespace {

struct ExecutionGuard {
    bool& executing;
    explicit ExecutionGuard(bool& value) : executing(value) { executing = true; }
    ~ExecutionGuard() { executing = false; }
};

}

bool EnemyVisuals::validBank(int bank) const {
    const int maximum=bankDomain_==BankDomain::ExtraStage16Rank4?4:BossBank;
    return bank >= 0 && bank <= maximum;
}

bool EnemyVisuals::rejectReentry(std::string* error) {
    if(operationFailure_.empty())operationFailure_="enemy ANM callback reentry is unsupported";
    if(error)*error=operationFailure_;
    return false;
}

bool EnemyVisuals::validVisibility(VisibilityEffect effect) {
    return effect == VisibilityEffect::None || effect == VisibilityEffect::Hide || effect == VisibilityEffect::Show;
}

void EnemyVisuals::applyVisibility(anm::State& state, VisibilityEffect effect) {
    if (effect == VisibilityEffect::Hide)
        state.flags &= ~2u;
    else if (effect == VisibilityEffect::Show)
        state.flags |= 2u;
}

EnemyVisuals::Nodes::iterator EnemyVisuals::erase(Nodes::iterator node) {
    auto owner = owners_.find(node->entity);
    if (owner != owners_.end() && --owner->second.nodes == 0)
        owners_.erase(owner);
    return nodes_.erase(node);
}

void EnemyVisuals::eraseChildren(uint32_t parent) {
    for(auto child=nodes_.begin();child!=nodes_.end();){
        if(child->parent==parent)child=erase(child);else ++child;
    }
}

const EnemyVisuals::Node* EnemyVisuals::findNode(uint32_t identity) const {
    const auto found=std::find_if(nodes_.begin(),nodes_.end(),[identity](const Node& node){return node.node==identity;});
    return found==nodes_.end()?nullptr:&*found;
}

bool EnemyVisuals::worldPose(const Node& node,Pose& pose) const {
    if(!node.failure.empty()||node.vm.faulted()||node.vm.state().ended||node.vm.state().deleteRequested)return false;
    const auto& state=node.vm.state();
    pose={state.x+state.engineX+state.offsetX,state.y+state.engineY+state.offsetY,state.z+state.engineZ+state.offsetZ,
        state.sx,state.sy,state.rotation,float(state.geometryRadius*state.sx)};
    if(node.parent){
        const auto* parent=findNode(node.parent);Pose inherited;
        if(!parent||parent->parent||!worldPose(*parent,inherited))return false;
        pose.x+=inherited.x;pose.y+=inherited.y;pose.z+=inherited.z;
        pose.sx*=inherited.sx;pose.sy*=inherited.sy;
        pose.radius*=inherited.sx;
        if(state.mode==1||state.mode==16)pose.rotation+=inherited.rotation;
    }
    return true;
}

void EnemyVisuals::spawnChild(Node& parent,const anm::ChildRequest& request) {
    const auto reject=[&](const std::string& reason){
        if(parent.failure.empty())parent.failure=reason;
        request.failure=parent.failure.c_str();
    };
    if(!parent.failure.empty()){request.failure=parent.failure.c_str();return;}
    const int ownerScript=parent.vm.state().script;
    const bool originalRequest=parent.bank==0&&request.bank==0&&request.opcode==88&&!request.detached
        &&!request.hasOffset&&request.registrationFlags==0&&(
            (ownerScript==182&&request.script>=176&&request.script<=181)
            ||(ownerScript==185&&request.script==184)
            ||(ownerScript==189&&((request.script>=176&&request.script<=181)||(request.script>=186&&request.script<=188)))
            ||(ownerScript==209&&request.script>=206&&request.script<=208)
            ||(ownerScript==133&&(request.script==132||request.script==121))
            ||((ownerScript==160||ownerScript==161)&&request.script==159)
            ||(ownerScript==123&&(request.script==121||request.script==122))
            ||(ownerScript==127&&(request.script==126||request.script==121))
            ||(ownerScript==125&&(request.script==124||request.script==121))
            ||(ownerScript==138&&(request.script==137||request.script==136))
            ||(ownerScript==197&&(request.script==194||request.script==195||request.script==196))
            ||(ownerScript==193&&(request.script==190||request.script==191||request.script==192))
            ||(ownerScript==201&&(request.script==198||request.script==199||request.script==200))
            ||(ownerScript==205&&(request.script==202||request.script==203||request.script==204))
            ||(ownerScript==213&&(request.script==210||request.script==211||request.script==212))
            ||(ownerScript==217&&(request.script==214||request.script==215||request.script==216)));
    const bool originalExtra148Request=bankDomain_==BankDomain::ExtraStage16Rank4&&parent.bank==0&&request.bank==0
        &&((ownerScript==148&&(request.script==147||request.script==136))
            ||(ownerScript==140&&(request.script==139||request.script==136)))&&request.opcode==88
        &&!request.detached&&!request.hasOffset&&request.registrationFlags==0;
    const bool originalExtraRequest=bankDomain_==BankDomain::ExtraStage16Rank4&&parent.bank==3&&request.bank==3
        &&request.opcode==88&&!request.detached&&!request.hasOffset&&request.registrationFlags==0
        &&(ownerScript==8||ownerScript==9||ownerScript==10)&&request.script==11;
    if(parent.parent||(!originalRequest&&!originalExtraRequest&&!originalExtra148Request)){
        reject("unimplemented original boss-child topology owner="+std::to_string(ownerScript)+" child="+std::to_string(request.script)
            +" bank="+std::to_string(request.bank)+" opcode="+std::to_string(request.opcode)
            +" flags="+std::to_string(request.registrationFlags)+" detached="+std::to_string(request.detached)
            +" offset="+std::to_string(request.hasOffset));return;
    }
    const auto reservation=findNode(parent.node)?0u:1u;
    if(!nextNode_||nodes_.size()>=MaxNodes-reservation){reject("enemy ANM child node/identity budget exceeded");return;}
    Node child;child.node=nextNode_++;child.parent=parent.node;child.entity=parent.entity;child.slot=parent.slot;child.bank=request.bank;
    child.vm.control().layer=parent.vm.state().layer;
    child.vm.control().engineX=request.x;child.vm.control().engineY=request.y;child.vm.control().engineZ=request.z;
    auto runtime=runtimeFor(child);std::string error;
    if(!child.vm.bind(registry_,child.bank,request.script,runtime)||!checked(child,error)){
        reject(error.empty()?"enemy ANM child binding failed":error);return;
    }
    if(child.vm.state().ended||child.vm.state().deleteRequested){reject("enemy ANM immediate child retirement is unproved");return;}
    ++owners_[child.entity].nodes;
    nodes_.push_back(std::move(child));
    request.createdState=&nodes_.back().vm.control();
}

bool EnemyVisuals::load(int bank, const void* bytes, size_t size, std::string& error) {
    error.clear();
    if (executing_) {
        return rejectReentry(&error);
    }
    if (!validBank(bank)&&bank!=11) {
        error = "enemy ANM bank must be native0..3";
        return false;
    }
    if (!registry_.loadBank(bank, static_cast<const uint8_t*>(bytes), size, error))
        return false;
    for (auto node = nodes_.begin(); node != nodes_.end();) {
        if (node->bank == bank)
            node = erase(node);
        else
            ++node;
    }
    return true;
}

bool EnemyVisuals::scriptSupported(int bank, int script, std::string& error) const {
    const auto resource = registry_.bank(bank);
    if (!resource || script < 0 || size_t(script) >= resource->scripts.size()) {
        error = "invalid enemy ANM bank/script binding";
        return false;
    }
    return true;
}

uint32_t EnemyVisuals::random32(Node& node, bool display) {
    if(node.failure.empty()&&!operationFailure_.empty())node.failure=operationFailure_;
    if (!node.failure.empty())
        return 0;
#if defined(__cpp_exceptions)
    try {
        const auto value=(display ? displayRandom32_ : gameRandom32_)();
        if(!operationFailure_.empty())node.failure=operationFailure_;
        return value;
    } catch (...) {
        node.failure = std::string("enemy ANM ") + (display ? "display" : "game") + " RNG callback threw";
        return 0;
    }
#else
    const auto value=(display ? displayRandom32_ : gameRandom32_)();
    if(!operationFailure_.empty())node.failure=operationFailure_;
    return value;
#endif
}

anm::Runtime EnemyVisuals::runtimeFor(Node& node) {
    anm::Runtime runtime;
    runtime.instructionBudget = InstructionBudget;
    runtime.updateControl=updateControl_;
    if (gameRandom32_)
        runtime.gameRandom32 = [this, &node] { return random32(node, false); };
    if (displayRandom32_)
        runtime.displayRandom32 = [this, &node] { return random32(node, true); };
    runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    runtime.unsupported = [&node](int bank, int script, int opcode, const char* reason) {
        if (node.failure.empty())
            node.failure = "enemy ANM entity=" + std::to_string(node.entity) + " slot=" + std::to_string(node.slot) +
                           " bank=" + std::to_string(bank) + " script=" + std::to_string(script) +
                           " opcode=" + std::to_string(opcode) + ": " + reason;
    };
    runtime.spawnChild = [this,&node](const anm::ChildRequest& request) {spawnChild(node,request);};
    const auto resource = registry_.bank(node.bank);
    runtime.remapSprite = [this,&node, resource](int sprite) {
        if(sprite<0){const auto font=registry_.bank(11);if(!font||font->sprites.size()<=313)if(node.failure.empty())node.failure="original negative-sprite ascii313 font is unavailable";return sprite;}
        if ((!resource || size_t(sprite) >= resource->sprites.size()) && node.failure.empty())
            node.failure = "enemy ANM unsupported sprite=" + std::to_string(sprite) +
                           "; original enemy font fallback is not established";
        return sprite;
    };
    return runtime;
}

bool EnemyVisuals::checked(Node& node, std::string& error) {
    if(node.failure.empty()&&!operationFailure_.empty())node.failure=operationFailure_;
    if (node.failure.empty() && node.vm.faulted()) {
        const auto diagnostic = node.vm.diagnostics();
        node.failure = "enemy ANM opcode=" + std::to_string(diagnostic.faultOpcode) + ": " + diagnostic.reason;
    }
    const auto& state = node.vm.state();
    if (node.failure.empty()) {
        for (const float value : {state.x, state.y, state.z, state.rx, state.ry, state.rotation,
                                 state.sx, state.sy, state.engineX, state.engineY, state.engineZ,
                                 state.offsetX, state.offsetY, state.offsetZ, state.u, state.v,
                                 state.uvScaleX, state.uvScaleY, state.geometryRadius, state.geometryWidth,
                                 state.spriteWidth, state.spriteHeight})
            if (!std::isfinite(value)) {
                node.failure = "enemy ANM non-finite presentation state";
                break;
            }
    }
    if (node.failure.empty() && state.sprite >= 0 && !spriteInfo(state.spriteBank, state.sprite))
        node.failure = "enemy ANM sprite resource is unavailable";
    if (node.failure.empty())
        return true;
    error = node.failure;
    return false;
}

bool EnemyVisuals::create(uint32_t entity, int slot, int bank, int script, int layer,
                          VisibilityEffect effect, std::string& error) {
    error.clear();
    if (executing_) {
        return rejectReentry(&error);
    }
    operationFailure_.clear();
    if (slot < 0 || size_t(slot) >= AnimationSlotCount || !validBank(bank) ||
        layer < 0 || layer > 255 || !validVisibility(effect)) {
        error = "invalid enemy ANM slot/bank/layer/visibility";
        return false;
    }
    if (!scriptSupported(bank, script, error))
        return false;
    auto previous = std::find_if(nodes_.begin(), nodes_.end(), [entity, slot](const Node& node) {
        return node.parent==0&&node.entity == entity && node.slot == slot;
    });
    if (!nextNode_||(previous == nodes_.end() && nodes_.size() >= MaxNodes)) {
        error = "enemy ANM node budget exceeded";
        return false;
    }
    Node candidate;
    candidate.node=nextNode_++;
    candidate.entity = entity;
    candidate.slot = slot;
    candidate.bank = bank;
    candidate.vm.control().layer = layer;
    auto runtime = runtimeFor(candidate);
    {
        ExecutionGuard guard(executing_);
        if (!candidate.vm.bind(registry_, bank, script, runtime) || !checked(candidate, error)) {
            eraseChildren(candidate.node);
            if (error.empty())
                error = "enemy ANM startup binding failed";
            return false;
        }
    }
    std::array<float, 3> pose{};
    if (const auto owner = owners_.find(entity); owner != owners_.end())
        pose = owner->second.position;
    if (previous != nodes_.end()) {
        eraseChildren(previous->node);
        erase(previous);
    }
    if (candidate.vm.state().ended || candidate.vm.state().deleteRequested) {
        eraseChildren(candidate.node);
        return true;
    }
    auto& state = candidate.vm.control();
    state.engineX = pose[0];
    state.engineY = pose[1];
    state.engineZ = pose[2];
    applyVisibility(state, effect);
    for(auto& child:nodes_)if(child.parent==candidate.node)applyVisibility(child.vm.control(),effect);
    auto& owner = owners_[entity];
    owner.position = pose;
    ++owner.nodes;
    nodes_.push_front(std::move(candidate));
    return true;
}

bool EnemyVisuals::position(uint32_t entity, float x, float y, float z) {
    if(executing_)return rejectReentry();
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
        return false;
    const auto owner = owners_.find(entity);
    if (owner == owners_.end())
        return false;
    owner->second.position = {x, y, z};
    for (auto& node : nodes_)
        if (node.entity == entity&&node.parent==0) {
            auto& state = node.vm.control();
            state.engineX = x;
            state.engineY = y;
            state.engineZ = z;
        }
    return true;
}

bool EnemyVisuals::visibility(uint32_t entity, VisibilityEffect effect) {
    if(executing_)return rejectReentry();
    if (!validVisibility(effect) || !owners_.contains(entity))
        return false;
    for (auto& node : nodes_)
        if (node.entity == entity)
            applyVisibility(node.vm.control(), effect);
    return true;
}

std::optional<Bounds> EnemyVisuals::primaryAppearanceBounds(uint32_t entity,int bank,int script,std::string& error){
    error.clear();if(executing_){rejectReentry(&error);return std::nullopt;}
    if(entity==0||!validBank(bank)||script<0){error="invalid primary appearance identity";return std::nullopt;}
    const auto primary=std::find_if(nodes_.begin(),nodes_.end(),[entity](const Node& node){return node.entity==entity&&node.slot==0&&node.parent==0;});
    if(primary==nodes_.end())return std::nullopt;
    const auto& state=primary->vm.state();
    if(primary->bank!=bank||state.bank!=bank||state.script!=script){error="primary appearance bank/script owner mismatch";return std::nullopt;}
    if(!checked(*primary,error))return std::nullopt;
    if(state.sprite<0)return Bounds{};
    if(!spriteInfo(state.spriteBank,state.sprite)){error="primary appearance sprite owner missing";return std::nullopt;}
    const auto bounds=scaledSpriteBounds(state.spriteWidth,state.spriteHeight,state.sx,state.sy);
    if(!bounds)error="primary appearance dimensions are outside the verified finite domain";
    return bounds;
}

std::optional<HitFeedbackPlan> EnemyVisuals::applyHitFeedback(uint32_t entity,const HitFeedbackInput& input,std::string& error){
    error.clear();if(executing_){rejectReentry(&error);return std::nullopt;}
    const auto root=std::find_if(nodes_.begin(),nodes_.end(),[entity](const Node& node){return node.entity==entity&&node.slot==0&&node.parent==0;});
    auto actual=input;actual.primaryPresent=root!=nodes_.end()&&!root->vm.state().ended&&!root->vm.state().deleteRequested;
    if(!actual.primaryPresent)return planHitFeedback(actual,0,0);
    if(!checked(*root,error))return std::nullopt;
    ExecutionGuard guard(executing_);
    const auto next=planHitFeedback(actual,root->vm.state().flags,root->vm.state().secondary);
    if(!next){error="original hit feedback requires its actual phase flags";return std::nullopt;}
    root->vm.control().flags=next->primaryFlags;root->vm.control().secondary=next->primaryColor;
    return next;
}

bool EnemyVisuals::tickAll(std::string& error) {
    error.clear();
    if (executing_) {
        return rejectReentry(&error);
    }
    operationFailure_.clear();
    for (auto& node : nodes_)
        if (!node.vm.state().deleteRequested&&!checked(node, error))
            return false;
    ExecutionGuard guard(executing_);
    for (auto node = nodes_.begin(); node != nodes_.end();) {
        if(node->vm.state().deleteRequested){node=erase(node);continue;}
        const auto successor=std::next(node);
        if(node->parent&&!findNode(node->parent)){erase(node);node=successor;continue;}
        auto runtime = runtimeFor(*node);
        node->vm.tick(runtime);
        if (!checked(*node, error))
            return false;
        if (node->vm.state().ended || node->vm.state().deleteRequested)
            erase(node);
        node=successor;
    }
    return true;
}

bool EnemyVisuals::retireSlot(uint32_t entity,int slot,std::string& error) {
    error.clear();if(executing_)return rejectReentry(&error);
    if(entity==0||slot<0||std::size_t(slot)>=AnimationSlotCount){error="invalid original animation slot retirement";return false;}
    for(auto& node:nodes_)if(node.entity==entity&&node.slot==slot){auto& state=node.vm.control();state.flags|=0x20000000u;state.deleteRequested=true;}
    return true;
}

bool EnemyVisuals::removeOwner(uint32_t entity) {
    if (executing_)
        return rejectReentry();
    for (auto node = nodes_.begin(); node != nodes_.end();) {
        if (node->entity == entity)
            node = erase(node);
        else
            ++node;
    }
    return true;
}

bool EnemyVisuals::setUpdateControl(anm::UpdateControl control) {
    if(executing_)return rejectReentry();
    updateControl_=control;return true;
}

bool EnemyVisuals::setRandomSources(std::function<uint32_t()> game, std::function<uint32_t()> display) {
    if (executing_)
        return rejectReentry();
    gameRandom32_ = std::move(game);
    displayRandom32_ = std::move(display);
    return true;
}

std::vector<EnemyVisuals::Snapshot> EnemyVisuals::snapshots() const {
    std::vector<Snapshot> result;
    result.reserve(nodes_.size());
    for (const auto& node : nodes_)
        if (node.failure.empty() && !node.vm.faulted() && !node.vm.state().ended &&
            !node.vm.state().deleteRequested) {
            const auto& state = node.vm.state();
            Pose pose;
            if(worldPose(node,pose))result.push_back({node.entity, node.slot, state, spriteInfo(state.spriteBank, state.sprite),node.node,node.parent,pose});
        }
    return result;
}

std::optional<anm::SpriteInfo> EnemyVisuals::spriteInfo(int bank, int sprite) const {
    const auto resource = registry_.bank(bank);
    if (!resource || sprite < 0 || size_t(sprite) >= resource->sprites.size())
        return std::nullopt;
    return resource->sprites[sprite];
}

}
