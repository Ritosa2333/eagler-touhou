/* Entry: 0x004125b0; symbol: FUN_004125b0; body bytes: 349 */

void __fastcall FUN_004125b0(void *param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  void *pvVar4;
  int *unaff_FS_OFFSET;
  int local_24 [2];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567bd5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004d9e30((char **)&DAT_005c4d40);
  *(undefined4 *)((int)param_1 + 0x34) = 1;
  FUN_00412810(param_1);
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)param_1 + 4),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar4 = (void *)FUN_0040c300(puVar3);
    FUN_004124b0(param_1,pvVar4);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_0040c080(local_24,8);
  FUN_00412280((void *)((int)param_1 + 0x1c),local_24);
  local_8 = 1;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_24,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_24);
    pvVar4 = (void *)FUN_0040c300(puVar3);
    FUN_004124b0(param_1,pvVar4);
    FUN_00411c30(local_24);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_24);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

