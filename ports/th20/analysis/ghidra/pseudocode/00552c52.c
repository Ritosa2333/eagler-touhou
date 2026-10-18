/* Entry: 0x00552c52; symbol: FUN_00552c52; body bytes: 185 */

void __cdecl FUN_00552c52(UINT param_1,undefined4 param_2,int param_3)

{
  code *pcVar1;
  bool bVar2;
  uint uVar3;
  undefined3 extraout_var;
  int *unaff_FS_OFFSET;
  undefined4 *local_28;
  int *local_24;
  undefined1 *local_20;
  int local_1c;
  uint local_18;
  undefined1 local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056b2b6;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_3 == 0) {
    uVar3 = FUN_00543b86();
    if ((char)uVar3 != '\0') {
      FUN_00552d3e(param_1);
    }
  }
  local_28 = &param_2;
  local_11 = 0;
  local_24 = &param_3;
  local_20 = &local_11;
  local_8 = 0;
  local_18 = 2;
  local_1c = 2;
  operator()<>(&local_1c,&local_28);
  if (param_3 == 0) {
    bVar2 = FUN_00558496();
    if (CONCAT31(extraout_var,bVar2) == 1) {
      local_18 = local_18 & 0xffffff00;
    }
    else {
      uVar3 = FUN_0055e109();
      local_18 = CONCAT31(local_18._1_3_,'\x01' - ((char)uVar3 != '\0'));
    }
    if (param_3 == 0) {
      FUN_00552d10(param_1,(char)local_18);
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

