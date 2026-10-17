/* Entry: 0x00449e40; symbol: FUN_00449e40; body bytes: 347 */

void __thiscall FUN_00449e40(void *this,int param_1)

{
  undefined4 uVar1;
  void *pvVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  int local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056834d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  for (local_1c = 0; local_1c < 1; local_1c = local_1c + 1) {
    pvVar2 = (void *)FUN_00449490((void *)((int)this + 0x6c8),local_1c);
    iVar3 = FUN_0044a090(pvVar2,param_1);
    if (iVar3 != 0) {
      FUN_0044f3d0(this,param_1,local_1c);
      iVar3 = FUN_00449490((void *)((int)this + 0x6c8),local_1c);
      FUN_00449fa0(iVar3);
    }
    pvVar2 = (void *)FUN_00449490((void *)((int)this + 0x6c8),local_1c);
    iVar3 = FUN_0044a210(pvVar2,param_1);
    if (iVar3 != 0) {
      FUN_0044f3d0(this,param_1,local_1c);
      iVar3 = FUN_00449490((void *)((int)this + 0x6c8),local_1c);
      FUN_00449fa0(iVar3);
    }
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

