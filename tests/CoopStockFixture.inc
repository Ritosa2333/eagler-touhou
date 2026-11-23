// Test-only initial state seam; this test binary must never be packaged.
extern "C" int th128_coop_test_stock(unsigned seat,int amount){
    if(!coopActive()||world->frame()!=0||seat>=coop->seats||amount<0||amount>th128::freeze::PerfectFreezeCapRaw)return 0;
    if(seat==0&&!perfectHud->charge(amount,operationError))return 0;
    seatCharge(seat)=amount;return 1;
}
extern "C" int th128_coop_test_stock_rules(){
    using namespace th128;
    const auto death=player::death::chargeAwardPrefix(49000);
    const auto text=hud::originalPercentHUD(20000,10000,50000,24.0f);
    return freeze::PerfectFreezeCapRaw==50000&&freeze::spendPerfectFreezeRaw(50000)==40000&&
      freeze::addPerfectFreezeRaw(49999,2000)==50000&&death&&death->updatedCharge==50000&&text&&(*text)[2].text=="500%";
}
