/* Entry: 0x0040bc60; symbol: FUN_0040bc60; body bytes: 136 */

void __fastcall FUN_0040bc60(undefined4 *param_1)

{
  char cVar1;
  undefined4 uVar2;
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
  uVar2 = FUN_0040c310(&DAT_005c0240,6);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  cVar1 = FUN_0040c3d0((int)param_1);
  if (cVar1 != '\0') {
    FUN_0040c160(param_1);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

