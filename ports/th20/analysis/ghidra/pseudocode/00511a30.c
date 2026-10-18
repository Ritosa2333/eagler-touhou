/* Entry: 0x00511a30; symbol: FUN_00511a30; body bytes: 186 */

void __fastcall FUN_00511a30(void *param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  char **ppcVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568d50;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_1c,8);
  FUN_00412280(param_1,local_1c);
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    ppcVar4 = (char **)FUN_0040c300(puVar3);
    FUN_00413b70(DAT_005b8894,ppcVar4);
    FUN_00411c30(local_1c);
  }
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

