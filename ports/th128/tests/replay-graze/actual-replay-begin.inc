int th128_world_replay_begin(unsigned seed,double seconds){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerFireConfigured||!playerMotion||seed>65536||debugRunTainted||!std::isfinite(seconds)||seconds<0){
        operationError="normal replay requires actual fresh gameplay resources and a finite host clock";return 0;
    }
    const unsigned stage=unsigned(stageIdForEcl(worldStartedName));const auto random=world->gameRandomState();
    const auto group=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!group){operationError="normal replay initial shot group is unavailable";return 0;}
    try{
        const bool receivedCarry=normalReplay&&stageCarry.normalReplay==normalReplay&&stageCarry.valid&&stageCarry.acknowledged&&stageCarry.consumed
            &&stageCarry.gameRandomRestored&&stageCarry.restoredGeneration==worldGeneration&&stageCarry.scalarRestored
            &&stageCarry.scalarRestoredGeneration==worldGeneration&&stageCarry.nextId==int(stage)
            &&stageCarry.targetWorldName==worldStartedName&&stageCarry.targetDifficulty==worldStartDifficulty;
        const auto boundSeed=th128::record::replayStageSeed(seed,random.seed,random.calls,receivedCarry);
        if(!normalReplay){normalReplay=std::make_shared<th128::record::NativeRunReplay>();normalReplayOrigin=seconds;normalReplayClock=seconds;}
        if(normalReplay->configuredFor(stage))throw std::runtime_error("normal replay stage already bound");
        normalReplay->begin(stage,unsigned(worldStartDifficulty),boundSeed,*group,unsigned(originalContinueCount),scoreRaw,pointValueRaw,motivationRaw,playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea,playerRaw.horizontal,playerRaw.vertical,grazeGlobals.grazeCount);
        if(normalRecord)normalRecord->beginStage(stage,unsigned(worldStartDifficulty),seconds);
        bindOrdinaryTerminalAfterReplayBegin();
        normalReplayClockPending=false;return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
