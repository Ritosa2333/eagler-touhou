/* Entry: 0x00447520; symbol: FUN_00447520; body bytes: 205 */

void FUN_00447520(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056828d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,1);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  uVar2 = (uint)((ulonglong)param_2 * 0x58);
  if ((int)((ulonglong)param_2 * 0x58 >> 0x20) != 0) {
    uVar2 = 0xffffffff;
  }
  iVar3 = FUN_00542a01(uVar2);
  if (iVar3 != 0) {
    FUN_0040bc20(iVar3,0x58,param_2,FUN_004488f0);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

