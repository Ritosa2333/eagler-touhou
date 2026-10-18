/* Entry: 0x004c0d00; symbol: FUN_004c0d00; body bytes: 264 */

void __fastcall FUN_004c0d00(int param_1)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined3 extraout_var;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056863d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar2 = FUN_0040c300((undefined4 *)&stack0x00000004);
  if (iVar2 != 0) {
    FUN_0040c080(local_1c,8);
    FUN_00412280((void *)(param_1 + 0xc414),local_1c);
    local_8 = 0;
    piVar3 = (int *)FUN_00412540();
    while (bVar1 = FUN_00411bb0(local_1c,piVar3), bVar1) {
      puVar4 = (undefined4 *)FUN_0040c300(local_1c);
      iVar2 = FUN_0040c300(puVar4);
      bVar1 = FUN_004c0210((void *)(iVar2 + 0x8c),(int *)&stack0x00000004);
      if (CONCAT31(extraout_var,bVar1) != 0) {
        FUN_0040c300(puVar4);
        local_8 = 0xffffffff;
        FUN_00411b00(local_1c);
        goto LAB_004c0ded;
      }
      FUN_00411c30(local_1c);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_1c);
  }
LAB_004c0ded:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

