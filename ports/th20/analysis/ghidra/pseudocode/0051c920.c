/* Entry: 0x0051c920; symbol: FUN_0051c920; body bytes: 253 */

void __thiscall FUN_0051c920(void *this,int param_1,int param_2,int param_3)

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
  puStack_c = &LAB_0056762d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  if ((((param_1 < 0) || (3 < param_1)) || (param_2 < 0)) ||
     (((1 < param_2 || (param_3 < 0)) || (8 < param_3)))) {
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  else {
    iVar2 = FUN_004640a0(0x5ba568);
    if (iVar2 == 4) {
      iVar2 = FUN_004640e0((int)this);
      *(int *)(iVar2 + 0x148 + param_2 * 0x10 + param_1 * 4) = param_3;
    }
    else {
      iVar2 = FUN_004640e0((int)this);
      *(int *)(iVar2 + 0x128 + param_2 * 0x10 + param_1 * 4) = param_3;
    }
    FUN_004beb40((int)this);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

