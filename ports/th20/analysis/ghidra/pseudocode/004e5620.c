/* Entry: 0x004e5620; symbol: FUN_004e5620; body bytes: 631 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004e5620(void *param_1)

{
  int iVar1;
  int iVar2;
  int local_48;
  int local_40;
  tm local_38;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(&local_14,DAT_0056fb7c,DAT_0056fa28,0);
  local_48 = FUN_0040c300((undefined4 *)((int)param_1 + 0x30));
  local_10 = (float)DAT_0056fa28;
  FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"            Score Ranking!!");
  local_10 = (float)local_48 * _DAT_00572644 + DAT_0056fe7c;
  local_14 = local_14 + _DAT_00572648;
  if (*(int *)((int)param_1 + 0xe4) == 0) {
    FUN_004e5020(param_1,local_14);
  }
  else {
    local_48 = -1;
  }
  local_14 = DAT_0056fb7c;
  local_10 = DAT_0056fe7c;
  local_38.tm_sec = 0;
  local_38.tm_min = 0;
  local_38.tm_hour = 0;
  local_38.tm_mday = 0;
  local_38.tm_mon = 0;
  local_38.tm_year = 0;
  local_38.tm_wday = 0;
  local_38.tm_yday = 0;
  local_38.tm_isdst = 0;
  for (local_40 = 0; local_40 < 10; local_40 = local_40 + 1) {
    if (local_40 == local_48) {
      FUN_00470ad0(DAT_005c0698,0xffffff00);
    }
    else {
      FUN_00470ad0(DAT_005c0698,0xff808080);
    }
    iVar1 = FUN_0050fc50(DAT_005c6108);
    iVar2 = FUN_004640a0(0x5ba568);
    iVar1 = iVar1 + 0x18 + iVar2 * 400 + local_40 * 0x28;
    if (*(int *)(iVar1 + 0x18) == 0 && *(int *)(iVar1 + 0x1c) == 0) {
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%2d %s %9llu%d --/--/-- Stage -");
    }
    else {
      FUN_004ac270(&local_38,(uint *)(iVar1 + 0x18));
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%2d %s %9llu%d %.2d/%.2d/%.2d %s");
    }
    local_10 = local_10 + _DAT_00572644;
  }
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

