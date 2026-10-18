/* Entry: 0x0052c440; symbol: FUN_0052c440; body bytes: 166 */

void __thiscall FUN_0052c440(void *this,int param_1,int param_2,int param_3)

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
  iVar2 = FUN_004bd460(this,param_2,param_3);
  FUN_00414580((void *)(iVar2 + 0x76d4),param_1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

