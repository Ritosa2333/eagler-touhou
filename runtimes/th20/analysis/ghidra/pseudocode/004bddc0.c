/* Entry: 0x004bddc0; symbol: FUN_004bddc0; body bytes: 208 */

void __thiscall FUN_004bddc0(void *this,uint param_1)

{
  undefined4 uVar1;
  uint *puVar2;
  int iVar3;
  undefined1 *puVar4;
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
  puVar2 = (uint *)FUN_00484980((void *)((int)this + 0x124284),*(uint *)((int)this + 0x124280));
  *puVar2 = param_1;
  *(int *)((int)this + 0x124280) = *(int *)((int)this + 0x124280) + 1;
  iVar3 = FUN_004640e0((int)this);
  puVar4 = (undefined1 *)FUN_004bd060((void *)(iVar3 + 0xe8),param_1);
  *puVar4 = 1;
  FUN_004beb40((int)this);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

