/* Entry: 0x005204c0; symbol: FUN_005204c0; body bytes: 268 */

undefined4 __fastcall FUN_005204c0(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  void *local_10;
  int local_c;
  void *local_8;
  
  local_c = *(int *)((int)param_1 + 0x20);
  local_8 = param_1;
  if (local_c == 0) {
    local_10 = (void *)((int)param_1 + 0x390);
    bVar1 = FUN_00449430(local_10,0);
    if (bVar1) {
      local_14 = FUN_00437950(DAT_005c0698);
      puVar2 = FUN_00450c70(&local_18,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar2;
    }
    FUN_0052cf80(local_8,0x2d);
    FUN_004bfc70();
    *(undefined4 *)((int)local_8 + 0x20) = 1;
    FUN_00423520((void *)((int)local_8 + 0x154),0);
    FUN_004e6b10(DAT_005c4d24,DAT_0056e050);
  }
  else if ((local_c == 1) && (iVar3 = FUN_004e6770((int)DAT_005c4d24), iVar3 != 0)) {
    FUN_00479040((void *)((int)local_8 + 0x390));
    FUN_004117a0((void *)((int)local_8 + 0x390),0);
    FUN_0052cc30(local_8,0x2d);
    FUN_0052ced0(local_8,1);
    FUN_004217c0(DAT_005c4d24);
  }
  return 0;
}

