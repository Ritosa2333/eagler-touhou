/* Entry: 0x0047d100; symbol: FUN_0047d100; body bytes: 320 */

void __thiscall FUN_0047d100(void *this,float *param_1,float param_2,int param_3)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  void *this_00;
  int iVar4;
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
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)this + 0x286d64),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,piVar2), bVar1) {
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    this_00 = (void *)FUN_0040c300(puVar3);
    if (((*(int *)((int)this_00 + 0x50) == 2) || (*(int *)((int)this_00 + 0x50) == 1)) &&
       (iVar4 = FUN_00485a60((void *)((int)this_00 + 100),param_1,
                             *(float *)((int)this_00 + 0x7c) / DAT_0056c8d0 + param_2), iVar4 != 0))
    {
      FUN_0047c8f0(this_00,param_3);
      *(int *)((int)this + 0x286d88) = *(int *)((int)this + 0x286d88) + 1;
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

