bool ordinaryCarrySessionPresent(bool restored){
    static constexpr std::array<int,6> terminals{3,5,8,10,13,15},previous{2,4,7,9,12,14};
    if(!world||world->faulted()||!stageControl.initialized()||!stageControl.sessionConstructed()
        ||stageControl.current()!=7||stageControl.gateSet()
        ||(stageControl.pending()!=7&&(!restored||stageControl.pending()!=15)))return false;
    const int stage=stageIdForEcl(worldStartedName);const auto selected=std::find(terminals.begin(),terminals.end(),stage);
    if(selected==terminals.end())return false;
    const auto& c=stageCarry;const auto route=std::size_t(selected-terminals.begin());
    if(!c.valid||!c.token||!c.sourceGeneration||c.sourceGeneration>=worldGeneration
        ||!c.targetBound||!c.preInitBound||!c.gameRandomRestored||c.restoredGeneration!=worldGeneration
        ||c.sourceCurrentId!=previous[route]||stageIdForEcl(c.sourceWorldName)!=c.sourceCurrentId
        ||c.nextId!=stage||c.targetWorldName!=worldStartedName
        ||c.sourceDifficulty!=worldStartDifficulty||c.difficulty!=worldStartDifficulty||c.targetDifficulty!=worldStartDifficulty
        ||!c.sourceTransitionSerial||!c.sourceControlActionRecreate||!c.sourceDestroyExisting
        ||c.sourceControlCurrent!=7||c.sourceControlPending!=7||c.sourceControlSessionArgument!=0
        ||!c.normalRecord||!c.normalReplay||c.debugRunTainted||c.debugInfiniteRespawn)return false;
    // Original state12 reconstructs the actual session with sessionFlag=0; the archived carry proves presence.
    if(!restored)return !c.consumed&&!c.acknowledged&&world->frame()==0;
    return c.scalarRestored&&c.scalarRestoredGeneration==worldGeneration&&c.acknowledged&&c.consumed
        &&c.nextProgress==currentStageProgress&&ordinaryCarryContinueCountMatches()
        &&normalRecord==c.normalRecord&&normalReplay==c.normalReplay;
}
