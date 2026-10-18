/* Entry: 0x0052c4f0; symbol: FUN_0052c4f0; body bytes: 331 */

void __thiscall FUN_0052c4f0(void *this,int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_00463fb0((int)this);
  iVar2 = FUN_0052c440(this,param_1,param_2,0);
  if ((((iVar2 != 0) && (iVar2 = FUN_0052c440(this,param_1,param_2,1), iVar2 != 0)) &&
      (iVar2 = FUN_0052c440(this,param_1,param_2,2), iVar2 != 0)) &&
     (((iVar2 = FUN_0052c440(this,param_1,param_2,3), iVar2 != 0 &&
       (iVar2 = FUN_0052c440(this,param_1,param_2,4), iVar2 != 0)) &&
      ((iVar2 = FUN_0052c440(this,param_1,param_2,5), iVar2 != 0 &&
       (iVar2 = FUN_0052c440(this,param_1,param_2,6), iVar2 != 0)))))) {
    FUN_0052c440(this,param_1,param_2,7);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

