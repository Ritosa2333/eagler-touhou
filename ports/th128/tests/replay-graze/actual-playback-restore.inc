int th128_world_replay_playback_restore(const std::uint8_t* data,unsigned length,int rank){
    operationError.clear();
    if(world->faulted()||worldTickExecuting||world->frame()!=0||!playerMotion||!playerFireConfigured||normalReplay
        ||debugRunTainted||rank!=worldStartDifficulty||length!=144||!th128::runtime::inputRange(data,length)){
        operationError="recorded stage restoration requires fresh replay-only ownership";return 0;
    }
    const auto bytes=std::span<const std::uint8_t>(data,length);
    const auto word=[&](unsigned offset){return th128::record::NativeReplaySlotPreview::word(bytes,offset);};
    const auto half=[&](unsigned offset){return unsigned(bytes[offset])|(unsigned(bytes[offset+1])<<8);};
    const int score=std::bit_cast<int>(word(12)),point=std::bit_cast<int>(word(20)),motivation=std::bit_cast<int>(word(128)),perfect=std::bit_cast<int>(word(132));
    const int graze=std::bit_cast<int>(word(0x28));
    const float area=std::bit_cast<float>(word(136));
    const auto group=th128::playershot::selectShotGroup(area,rank);
    const auto random=world->gameRandomState();
    if(half(0)!=unsigned(stageIdForEcl(worldStartedName))||half(2)!=random.seed||!group||half(16)!=*group
        ||score<0||score>999999999||point<0||point>1000000000||motivation<0||motivation>100000||perfect<0||perfect>30000
        ||graze<0||graze>1000000000
        ||word(32)>9||!std::isfinite(area)||area<0||area>1048576||cumulativeFrozenArea!=area
        ||std::bit_cast<int>(word(24))!=playerRaw.horizontal||std::bit_cast<int>(word(28))!=playerRaw.vertical){
        operationError="recorded stage header resources or original initialized owners differ";return 0;
    }
    if(!perfectHud->charge(perfect,operationError))return 0;
    scoreRaw=score;pointValueRaw=point;motivationRaw=motivation;motivationFlag=0;
    grazeGlobals.grazeCount=graze;
    playerWindowControl.perfectCharge4b4d70=perfect;originalContinueCount=int(word(32));return 1;
}
