/* Entry: 0x0047cf60; symbol: FUN_0047cf60; body bytes: 401 */

void __thiscall
FUN_0047cf60(void *this,float *param_1,float param_2,int param_3,int param_4,uint param_5)

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
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005685fd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar2 = (void *)FUN_00485970((int)this);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar2,local_1c);
  local_8 = 0;
  piVar3 = (int *)FUN_00412540();
  do {
    bVar1 = FUN_00411bb0(local_1c,piVar3);
    if (!bVar1) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
LAB_0047d0d6:
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    puVar4 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar2 = (void *)FUN_0040c300(puVar4);
    if ((((*(int *)((int)pvVar2 + 0x50) == 2) || (*(int *)((int)pvVar2 + 0x50) == 1)) &&
        (*(int *)((int)pvVar2 + 0x18) == 0)) &&
       (iVar5 = FUN_00485a60((void *)((int)pvVar2 + 100),param_1,
                             *(float *)((int)pvVar2 + 0x7c) / DAT_0056c8d0 + param_2), iVar5 != 0))
    {
      *(uint *)((int)pvVar2 + 0x14) =
           *(uint *)((int)pvVar2 + 0x14) & 0xffffe7ff | (param_5 & 3) << 0xb;
      FUN_0047c8f0(pvVar2,param_3);
      param_4 = param_4 + -1;
      *(int *)((int)this + 0x286d88) = *(int *)((int)this + 0x286d88) + 1;
      if (param_4 < 1) {
        local_8 = 0xffffffff;
        FUN_00411b00(local_1c);
        goto LAB_0047d0d6;
      }
    }
    FUN_00411c30(local_1c);
  } while( true );
}

