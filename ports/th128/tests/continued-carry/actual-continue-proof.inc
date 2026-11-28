struct OrdinaryCarryContinueRecord {
    unsigned generation=0,token=0,pauseSerial=0;
    int baseCount=0,currentCount=0;
    std::weak_ptr<th128::record::NativeNormalRecord> record;
    std::weak_ptr<th128::record::NativeRunReplay> replay;
};
OrdinaryCarryContinueRecord ordinaryCarryContinue;
bool ordinaryCarryContinueCountMatches(){
    if(stageCarry.originalContinueCount==originalContinueCount)return true;
    const auto& committed=ordinaryCarryContinue;
    return originalContinueCount>stageCarry.originalContinueCount&&originalContinueCount<=5
        &&committed.generation==worldGeneration&&committed.token==stageCarry.token&&committed.pauseSerial
        &&committed.baseCount==stageCarry.originalContinueCount&&committed.currentCount==originalContinueCount
        &&committed.record.lock()==normalRecord&&committed.replay.lock()==normalReplay;
}
