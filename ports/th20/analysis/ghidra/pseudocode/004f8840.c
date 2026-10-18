/* Entry: 0x004f8840; symbol: FUN_004f8840; body bytes: 683 */

void __fastcall FUN_004f8840(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar2;
  undefined1 local_20;
  undefined1 local_1c;
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((0 < *(int *)((int)param_1 + 0x30)) &&
     (*(int *)((int)param_1 + 0x34) < *(int *)((int)param_1 + 0x30))) {
    bVar1 = FUN_004ff7e0(param_1);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      *(undefined4 *)((int)param_1 + 0x34) = *(undefined4 *)((int)param_1 + 0x30);
      FUN_00423520((void *)((int)param_1 + 0x10),0x78);
    }
  }
  if (*(int *)((int)param_1 + 0x34) != 0) {
    FUN_00422dd0(local_14,DAT_0056fc04,DAT_00573168,0);
    FUN_00470ad0(DAT_005c0698,0xffffff00);
    bVar1 = FUN_004ff7e0(param_1);
    if (CONCAT31(extraout_var_00,bVar1) == 0) {
      iVar2 = FUN_00472040((void *)((int)param_1 + 0x10),6);
      if (iVar2 < 3) {
        FUN_00470ad0(DAT_005c0698,0xffffff00);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0x80808080);
      }
    }
    if (*(char *)((int)param_1 + 0x50) == '\0') {
      local_1c = 0xff;
    }
    else {
      local_1c = 0x40;
    }
    FUN_00488940(DAT_005c0698,local_1c);
    if (*(char *)((int)param_1 + 0x50) == '\0') {
      local_20 = 0xff;
    }
    else {
      local_20 = 0x40;
    }
    FUN_004b8600(DAT_005c0698,local_20);
    FUN_00488b00(DAT_005c0698,6);
    FUN_00488a00(DAT_005c0698,0);
    FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
    FUN_004b8620(DAT_005c0698,2,2);
    FUN_0046cc80(DAT_005c0698,local_14,*(uint *)((int)param_1 + 0x34));
    FUN_004b8620(DAT_005c0698,1,2);
    FUN_004b8d90(DAT_005c0698,DAT_00573160,DAT_00573160);
    if (*(int *)((int)param_1 + 0x34) < 2) {
      FUN_0040ddd0(&stack0xffffffd4,"hit");
      FUN_0046ce90(DAT_005c0698,local_14);
    }
    else {
      FUN_0040ddd0(&stack0xffffffd4,"hits");
      FUN_0046ce90(DAT_005c0698,local_14);
    }
    FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_00488940(DAT_005c0698,0xff);
    FUN_004b8600(DAT_005c0698,0xff);
    FUN_00488a00(DAT_005c0698,0);
    FUN_004b8620(DAT_005c0698,1,1);
    FUN_00488b00(DAT_005c0698,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

