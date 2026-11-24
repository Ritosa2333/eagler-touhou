// Test-only initial state seam; this test binary must never be packaged.
extern "C" int th128_coop_test_stock(unsigned seat,int amount){
    if(!coopActive()||world->frame()!=0||seat>=coop->seats||amount<0||amount>th128::freeze::PerfectFreezeCapRaw)return 0;
    if(seat==0&&!perfectHud->charge(amount,operationError))return 0;
    seatCharge(seat)=amount;return 1;
}
extern "C" int th128_coop_test_stock_rules(){
    using namespace th128;
    const auto death=player::death::chargeAwardPrefix(19000);
    const auto text=hud::originalPercentHUD(20000,10000,20000,24.0f);
    return freeze::PerfectFreezeCapRaw==20000&&freeze::spendPerfectFreezeRaw(20000)==10000&&
      freeze::addPerfectFreezeRaw(19999,2000)==20000&&death&&death->updatedCharge==20000&&text&&(*text)[2].text=="200%";
}
extern "C" int th128_coop_test_overlap(unsigned seat){
    if(!coopActive()||seat>=coop->seats)return -1;
    const auto pos=th128::player::toPixels(seatPose(seat));
    return seatBody(seat).confirmedOverlap({pos.horizontal,pos.vertical,0},playerHitControl,operationError).value_or(-1);
}
