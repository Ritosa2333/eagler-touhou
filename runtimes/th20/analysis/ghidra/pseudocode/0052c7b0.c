/* Entry: 0x0052c7b0; symbol: FUN_0052c7b0; body bytes: 180 */

void __fastcall FUN_0052c7b0(void *param_1)

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
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar2 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  FUN_00463fb0((int)param_1);
  cVar1 = FUN_0052c870(param_1,0);
  if (cVar1 == '\0') {
    FUN_0052c870(param_1,1);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

