/* Entry: 0x00528a50; symbol: FUN_00528a50; body bytes: 1380 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00528a50(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_20;
  undefined4 local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = *(int *)(param_1 + 0x20);
  if (((iVar1 == 1) || (iVar1 == 2)) || ((iVar1 == 3 && (*(int *)(param_1 + 0x18) != 0x14)))) {
    FUN_00422dd0(&local_14,DAT_00575684,DAT_00570ae0,0);
    FUN_00488b00(DAT_005c0698,2);
    FUN_004e6a20(DAT_005c0698,1);
    for (local_20 = 0; local_20 < 5; local_20 = local_20 + 1) {
      iVar1 = *(int *)(param_1 + 0x58e4 + local_20 * 4);
      if (-1 < iVar1) {
        if ((*(int *)(param_1 + 0x18) == 0x14) &&
           (iVar2 = FUN_0040c300((undefined4 *)(param_1 + 0x24)), local_20 == iVar2)) {
          iVar2 = FUN_004b8340(0x5ba568);
          iVar3 = FUN_004b7f70(0x5ba568);
          iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
          iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
          if (*(int *)(iVar2 + 0xc4) == 0) {
            FUN_00470ad0(DAT_005c0698,0xffb0b0b0);
          }
          else {
            FUN_00470ad0(DAT_005c0698,0xff90d0ff);
          }
        }
        else {
          iVar2 = FUN_004b8340(0x5ba568);
          iVar3 = FUN_004b7f70(0x5ba568);
          iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
          iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
          if (*(int *)(iVar2 + 0xc4) == 0) {
            FUN_00470ad0(DAT_005c0698,0xff404040);
          }
          else {
            FUN_00470ad0(DAT_005c0698,0xff60a0c0);
          }
        }
        iVar2 = FUN_00488700((int)DAT_005c6108);
        iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
        if (*(int *)(iVar2 + 200) == 0) {
          iVar2 = FUN_00488700((int)DAT_005c6108);
          iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
          if (*(int *)(iVar2 + 0xcc) == 0) {
            FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"SCORE        00  ----/----");
            goto LAB_00528ae6;
          }
        }
        iVar2 = FUN_004b8340(0x5ba568);
        iVar3 = FUN_004b7f70(0x5ba568);
        iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
        FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
        iVar2 = FUN_004b8340(0x5ba568);
        iVar3 = FUN_004b7f70(0x5ba568);
        iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
        FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
        iVar2 = FUN_004b8340(0x5ba568);
        iVar3 = FUN_004b7f70(0x5ba568);
        iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
        FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
        FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"SCORE %8llu0  %4d/%4d");
        iVar2 = FUN_004b8340(0x5ba568);
        iVar3 = FUN_004b7f70(0x5ba568);
        iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
        iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
        if (*(int *)(iVar2 + 0xc0) == 0) {
          FUN_00470ad0(DAT_005c0698,0xff404040);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xff206060);
        }
        local_10 = local_10 + DAT_0056e734;
        if (*(int *)(&DAT_005af058 + iVar1 * 4) < 5) {
          iVar2 = FUN_004b8340(0x5ba568);
          iVar3 = FUN_004b7f70(0x5ba568);
          iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
          FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
          iVar2 = FUN_004b8340(0x5ba568);
          iVar3 = FUN_004b7f70(0x5ba568);
          iVar2 = FUN_004bd460(DAT_005c6108,iVar3,iVar2);
          FUN_00486e10((void *)(iVar2 + 0xb08),iVar1);
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"GAME MODE        %4d/%4d");
        }
        local_10 = local_10 - DAT_0056e734;
      }
LAB_00528ae6:
      local_10 = local_10 + _DAT_00570acc;
    }
    FUN_00488b00(DAT_005c0698,0);
    FUN_004e6a20(DAT_005c0698,0);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

