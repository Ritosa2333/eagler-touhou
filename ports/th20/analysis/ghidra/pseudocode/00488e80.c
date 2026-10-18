/* Entry: 0x00488e80; symbol: FUN_00488e80; body bytes: 291 */

void __fastcall FUN_00488e80(int param_1)

{
  bool bVar1;
  int iVar2;
  void *pvVar3;
  int *piVar4;
  undefined4 *puVar5;
  void *this;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005685fd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar2 = FUN_0040c300(*(undefined4 **)(param_1 + 0x2ec));
  pvVar3 = (void *)FUN_00485970(iVar2);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar3,local_1c);
  local_8 = 0;
  piVar4 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar4);
    if (!bVar1) break;
    puVar5 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar3 = (void *)FUN_0040c300(puVar5);
    FUN_0049cae0(pvVar3,DAT_0056c8cc);
    this = (void *)FUN_00421810((int)pvVar3);
    FUN_0044fa10(this,0xffffffff);
    FUN_0049c260((int)pvVar3);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  pvVar3 = (void *)FUN_00460830(0);
  FUN_00479080(pvVar3,DAT_0056c8cc);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

