/* Entry: 0x004bc480; symbol: FUN_004bc480; body bytes: 231 */

void __fastcall FUN_004bc480(int param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
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
  FUN_00412280((void *)(param_1 + 0x10),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    iVar4 = FUN_0040c300(puVar3);
    FUN_00411ce0(iVar4 + 8);
    piVar5 = (int *)FUN_0040c300(puVar3);
    FUN_004b9aa0(piVar5);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *(undefined4 *)(param_1 + 0x28) = 0;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

