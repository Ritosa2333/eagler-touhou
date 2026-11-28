int exactProductionStep(){
    if(perfectConfigured){
        auto perfectStatus=perfectHost->status();
        if(perfectStatus.failed){world->reportRuntimeFault("perfect freeze host failed earlier");return 0;}
        const auto makePerfectContext=[&](th128::perfect::LaserContext lasers,int laserCount){
            th128::perfect::Context context;
            context.perfectCharge=playerWindowControl.perfectCharge4b4d70;
            const auto position=world->playerAimPosition();
            context.x=position.x;context.y=position.y;context.radius=playerProfileRadius;
            context.keys=playerInput.holding;
            context.xPressed=(playerInput.pressed&th128::perfect::XKeyBit)!=0;
            context.systemPresent=playerWindowControl.present;
            context.systemMode3c=playerWindowControl.mode3c;
            context.busy=iceHost?int(iceHost->statistics().busy):0;
            context.playerState=playerHit->snapshot().gate.stateF78;
            context.mode=th128::perfect::Mode::Normal;
            context.lasers=lasers;context.laserCount=laserCount;
            return context;
        };
        if(!perfectStatus.active&&(playerInput.holding&th128::perfect::XKeyBit)){
            std::string denied;
            const auto laserCount=world->laserObjectCount();
            if(!laserCount){
                perfectHost->reject("laser object count is unknown",denied);
            }else if(*laserCount!=0){
                perfectHost->reject("populated lasers are outside the supported Perfect domain",denied);
            }else if(iceHost->statistics().busy!=0||iceHost->statistics().objects!=0){
                perfectHost->reject("ordinary ice batch or live Perfect objects are nonempty",denied);
            }else{
                if(perfectHost->activate(makePerfectContext(th128::perfect::LaserContext::Empty,0),denied)){
                    if(grazeSounds.size()>65534){world->reportRuntimeFault("perfect activation sound journal budget");return 0;}
                    const float perfectSoundX=world->playerAimPosition().x;
                    grazeSounds.push_back({46,perfectSoundX,0});
                    grazeSounds.push_back({51,perfectSoundX,0});
                    const auto activated=perfectHost->status();
                    playerWindowControl.perfectCharge4b4d70=activated.charge;
                    playerWindowControl.mode3c=1;perfectModeOwned=true;
                    world->setPhaseSpellPlayerMode(playerWindowControl.mode3c);
                    if(!perfectHud->charge(activated.charge,operationError)){
                        world->reportRuntimeFault(operationError);return 0;
                    }
                    perfectActivatedThisTick=true;
                }else if(perfectHost->status().failed){
                    world->reportRuntimeFault(denied);return 0;
                }
            }
        }
    }
    return 1;
}
