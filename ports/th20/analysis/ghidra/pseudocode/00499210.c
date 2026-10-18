/* Entry: 0x00499210; symbol: FUN_00499210; body bytes: 219 */

void __fastcall FUN_00499210(int param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056838d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x108),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,piVar2), bVar1) {
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    FUN_0040c300(puVar3);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

