int th128_world_pause_active(){return originalPauseMenu&&originalPauseMenu->state().active?1:0;}
int th128_world_pause_commit(int action){
    operationError.clear();
    if(!th128_world_pause_active()||worldTickExecuting||soundJournalWindowSerial||!grazeSounds.empty()){
        operationError="pause action requires active owner and durable sound acknowledgement";return 0;
    }
    if(action!=int(th128::pause::Action::Resume)&&action!=int(th128::pause::Action::Continue)
        &&action!=int(th128::pause::Action::Title)&&action!=int(th128::pause::Action::Restart)){
        operationError="replay action requires its complete durable recorder consumer";return 0;
    }
    try{
        const bool savedExit=action==int(th128::pause::Action::Title)&&normalReplaySave&&normalReplaySave->finished()&&!normalReplaySave->gameOver();
        if(!savedExit&&int(originalPauseMenu->state().action)!=action)throw std::runtime_error("pause action serial/identity mismatch");
        const unsigned pauseSerial=originalPauseMenu->state().serial;
        const int previousContinues=originalContinueCount;
        const bool carryContinue=action==int(th128::pause::Action::Continue)&&previousContinues>=0&&previousContinues<5
            &&pauseSerial&&!debugRunTainted&&!debugInfiniteRespawnRun&&!replayPlayback.active()&&ordinaryCarrySessionPresent(true)
            &&(previousContinues==stageCarry.originalContinueCount||pauseSerial>ordinaryCarryContinue.pauseSerial);
        if(action==int(th128::pause::Action::Continue)){
            if(originalPauseMenu->state().kind!=th128::pause::Kind::GameOver||motivationRaw>=0)
                throw std::runtime_error("continue requires actual Game Over resources");
            const auto resources=th128::pause::continueResources({scoreRaw,motivationRaw,motivationFlag,playerWindowControl.perfectCharge4b4d70,originalContinueCount});
            scoreRaw=resources.score;motivationRaw=resources.motivation;motivationFlag=resources.motivationFlag;
            playerWindowControl.perfectCharge4b4d70=resources.perfect;originalContinueCount=resources.continues;
        }
        originalPauseMenu->commit(savedExit?th128::pause::Action::ReplaySave:th128::pause::Action(action));
        if(carryContinue&&originalContinueCount==previousContinues+1)
            ordinaryCarryContinue={worldGeneration,stageCarry.token,pauseSerial,stageCarry.originalContinueCount,
                originalContinueCount,normalRecord,normalReplay};
        if(savedExit){normalReplaySave.reset();normalReplayBytes.clear();}
        playerInput={};return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
