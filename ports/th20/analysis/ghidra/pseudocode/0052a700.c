/* Entry: 0x0052a700; symbol: FUN_0052a700; body bytes: 402 */

void __fastcall FUN_0052a700(int param_1)

{
  int iVar1;
  uint uVar2;
  int local_24;
  float local_20;
  float local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = *(int *)(param_1 + 0x20);
  if (((iVar1 == 2) || (iVar1 == 3)) || (iVar1 == 4)) {
    FUN_00422dd0(&local_14,DAT_00575670,DAT_00573fdc,0);
    FUN_00422e10(&local_20);
    FUN_00488b00(DAT_005c0698,7);
    FUN_004b8620(DAT_005c0698,0,1);
    FUN_004e6a20(DAT_005c0698,0);
    for (local_24 = 0; local_24 < 10; local_24 = local_24 + 1) {
      local_20 = local_14;
      local_1c = local_10;
      local_18 = local_c;
      iVar1 = FUN_0040ff90(param_1 + 0x5904);
      uVar2 = FUN_0040ff90(param_1 + 0x5914);
      FUN_0046b590(&local_14,local_24,(void *)(param_1 + 0x24),uVar2,iVar1);
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)(&PTR_s_Game_Start_005750a0)[local_24]);
      local_14 = local_20;
      local_c = local_18;
      local_10 = local_1c + DAT_0056f7b0;
    }
    FUN_004b8620(DAT_005c0698,1,1);
    FUN_00488b00(DAT_005c0698,0);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

