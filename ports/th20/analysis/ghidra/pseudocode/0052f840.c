/* Entry: 0x0052f840; symbol: FUN_0052f840; body bytes: 151 */

void __thiscall FUN_0052f840(void *this,uint param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined1 *puVar3;
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
  iVar2 = FUN_004640e0((int)this);
  puVar3 = (undefined1 *)FUN_0052c1b0((void *)(iVar2 + 0x68),param_1);
  *puVar3 = 1;
  FUN_004beb40((int)this);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

