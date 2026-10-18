/* Entry: 0x0049bf00; symbol: FUN_0049bf00; body bytes: 244 */

void __thiscall FUN_0049bf00(void *this,int param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056838d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != 0) {
    FUN_0040c080(local_1c,8);
    FUN_00412280((void *)((int)this + 0x108),local_1c);
    local_8 = 0;
    piVar2 = (int *)FUN_00412540();
    while (bVar1 = FUN_00411bb0(local_1c,piVar2), bVar1) {
      puVar3 = (undefined4 *)FUN_0040c300(local_1c);
      iVar4 = FUN_0040c300(puVar3);
      iVar4 = FUN_0040c300((undefined4 *)(iVar4 + 0x88));
      if (iVar4 == param_1) {
        local_8 = 0xffffffff;
        FUN_00411b00(local_1c);
        goto LAB_0049bfd9;
      }
      FUN_00411c30(local_1c);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_1c);
  }
LAB_0049bfd9:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

