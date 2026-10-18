/* Entry: 0x00463fe0; symbol: FUN_00463fe0; body bytes: 159 */

void __fastcall FUN_00463fe0(int param_1)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
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
  uVar3 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar3);
  local_8 = 0;
  cVar1 = *(char *)(param_1 + 0x1d1);
  cVar2 = FUN_00463f20(param_1);
  if (cVar1 != cVar2) {
    FUN_00464530(0x5b6758);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

