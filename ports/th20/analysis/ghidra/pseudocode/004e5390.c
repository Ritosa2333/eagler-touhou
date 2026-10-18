/* Entry: 0x004e5390; symbol: FUN_004e5390; body bytes: 300 */

void FUN_004e5390(undefined4 param_1,undefined4 *param_2,int param_3)

{
  tm local_2c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_2c.tm_sec = 0;
  local_2c.tm_min = 0;
  local_2c.tm_hour = 0;
  local_2c.tm_mday = 0;
  local_2c.tm_mon = 0;
  local_2c.tm_year = 0;
  local_2c.tm_wday = 0;
  local_2c.tm_yday = 0;
  local_2c.tm_isdst = 0;
  FUN_004ac270(&local_2c,(uint *)(param_3 + 0x10));
  if ((*(byte *)(param_3 + 10) >> 1 & 1) == 0) {
    FUN_0046c990(DAT_005c0698,param_2,(wchar_t *)"No.%.2d %s %.2d/%.2d/%.2d %s %s %s");
  }
  else {
    FUN_0046c990(DAT_005c0698,param_2,(wchar_t *)"No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d");
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

