/* Entry: 0x0051c7e0; symbol: FUN_0051c7e0; body bytes: 149 */

void __thiscall FUN_0051c7e0(void *this,uint param_1,undefined4 param_2)

{
  undefined4 uVar1;
  void *this_00;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005679cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  this_00 = (void *)FUN_004640e0((int)this);
  FUN_0051c880(this_00,param_1,param_2);
  FUN_004beb40((int)this);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

