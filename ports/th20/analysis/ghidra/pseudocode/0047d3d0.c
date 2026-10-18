/* Entry: 0x0047d3d0; symbol: FUN_0047d3d0; body bytes: 419 */

void __thiscall FUN_0047d3d0(void *this,float *param_1,int param_2)

{
  int iVar1;
  void *pvVar2;
  uint uVar3;
  
  if (param_2 == 0) {
    return;
  }
  iVar1 = FUN_00485390(param_1,DAT_0056cd94,DAT_0056cd94);
  if (iVar1 != 0) {
    return;
  }
  iVar1 = FUN_0040c300(*(undefined4 **)((int)this + 0x520));
  *(int *)(iVar1 + 0x286d80) = *(int *)(iVar1 + 0x286d80) + 1;
  if (param_2 == 1) {
    pvVar2 = (void *)FUN_0041cab0(*(int *)((int)this + 0x520));
    FUN_004c45b0(pvVar2,param_1,1,0xd);
    return;
  }
  if (param_2 == 4) {
    iVar1 = FUN_0040c300(*(undefined4 **)((int)this + 0x520));
    if (((*(int *)(iVar1 + 0x286d80) % 5 == 0) ||
        ((iVar1 = FUN_0040c300(*(undefined4 **)((int)this + 0x520)),
         *(int *)(iVar1 + 0x286d80) % 5 == 2 && (uVar3 = FUN_00423ee0(0x5ba4a8), uVar3 % 2 == 0))))
       || ((iVar1 = FUN_0040c300(*(undefined4 **)((int)this + 0x520)),
           *(int *)(iVar1 + 0x286d80) % 5 == 4 && (uVar3 = FUN_00423ee0(0x5ba4a8), uVar3 % 3 == 0)))
       ) {
      pvVar2 = (void *)FUN_0041cab0(*(int *)((int)this + 0x520));
      FUN_004c3c90(pvVar2,2,param_1,0xffffffff,DAT_0056e0f8 / DAT_0056c8d0,DAT_0056fd34,0,0,
                   0xffffffff);
    }
    else {
      pvVar2 = (void *)FUN_0041cab0(*(int *)((int)this + 0x520));
      FUN_004c45b0(pvVar2,param_1,1,0xd);
    }
    return;
  }
  return;
}

