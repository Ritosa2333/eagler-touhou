/* Entry: 0x0047d240; symbol: FUN_0047d240; body bytes: 388 */

void __thiscall FUN_0047d240(void *this,float *param_1,float param_2)

{
  bool bVar1;
  void *pvVar2;
  int *piVar3;
  undefined4 *puVar4;
  int iVar5;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  puStack_c = &LAB_005690d5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  pvVar2 = (void *)FUN_00485970((int)this);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar2,local_1c);
  local_8 = CONCAT31(local_8._1_3_,1);
  piVar3 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,piVar3), bVar1) {
    puVar4 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar2 = (void *)FUN_0040c300(puVar4);
    if ((((*(int *)((int)pvVar2 + 0x50) == 2) || (*(int *)((int)pvVar2 + 0x50) == 1)) &&
        (*(int *)((int)pvVar2 + 0x18) == 0)) &&
       ((iVar5 = FUN_00485a60((void *)((int)pvVar2 + 100),param_1,
                              *(float *)((int)pvVar2 + 0x7c) / DAT_0056c8d0 + param_2), iVar5 != 0
        && (iVar5 = FUN_00449630(&stack0x0000000c,(int)pvVar2 + 100), iVar5 != 0)))) {
      *(uint *)((int)pvVar2 + 0x14) = *(uint *)((int)pvVar2 + 0x14) & 0xffffe7ff;
      FUN_0047c8f0(pvVar2,0);
      *(int *)((int)this + 0x286d88) = *(int *)((int)this + 0x286d88) + 1;
    }
    FUN_00411c30(local_1c);
  }
  local_8 = local_8 & 0xffffff00;
  FUN_00411b00(local_1c);
  local_8 = 0xffffffff;
  FUN_00414400(&stack0x0000000c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

