/* Entry: 0x0047cb90; symbol: FUN_0047cb90; body bytes: 242 */

void __thiscall FUN_0047cb90(void *this,int param_1)

{
  bool bVar1;
  void *pvVar2;
  int *piVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056863d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar2 = (void *)FUN_00485970((int)this);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar2,local_1c);
  local_8 = 0;
  piVar3 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,piVar3), bVar1) {
    puVar4 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar2 = (void *)FUN_0040c300(puVar4);
    if ((*(int *)((int)pvVar2 + 0x50) != 0) && (*(int *)((int)pvVar2 + 0x50) != 3)) {
      FUN_0047c8f0(pvVar2,param_1);
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

