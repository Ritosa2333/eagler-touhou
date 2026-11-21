int th128_world_replay_playback_load(const void* data,unsigned length,int rank){
    operationError.clear();
    if(world->faulted()||worldTickExecuting||world->frame()!=0||!playerConfigured||!playerMotion||!playerFireConfigured
        ||normalReplay||debugRunTainted||rank!=worldStartDifficulty||!th128::runtime::inputRange(data,length)){
        operationError="replay playback requires actual fresh non-recording World with matching rank";return 0;
    }
    const auto group=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!group){operationError="replay playback original shot group unavailable";return 0;}
    const auto random=world->gameRandomState();
    try{
        const th128::record::ReplayPlaybackInitial actual{
            unsigned(stageIdForEcl(worldStartedName)),unsigned(worldStartDifficulty),random.seed,*group,
            unsigned(originalContinueCount),worldGeneration,unsigned(world->frame()),scoreRaw,pointValueRaw,
            playerRaw.horizontal,playerRaw.vertical,motivationRaw,playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea,grazeGlobals.grazeCount};
        replayPlayback.bind({static_cast<const std::uint8_t*>(data),length},actual);return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
