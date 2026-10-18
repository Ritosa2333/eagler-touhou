/* Entry: 0x0053f260; symbol: FUN_0053f260; body bytes: 265 */

undefined4 __thiscall FUN_0053f260(void *this,uint param_1,uint *param_2,char param_3)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  uint *puVar4;
  
  uVar2 = FUN_0053f750((int)this);
  iVar1 = *(int *)((int)this + 0x10) + 4 + param_1;
  if (uVar2 < (uint)((int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2)) {
    iVar1 = *(int *)((int)this + 0x10) + 4 + param_1;
    FUN_0053f370(this,(int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2);
  }
  if (param_3 != '\0') {
    piVar3 = (int *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                           (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    *piVar3 = (int)param_3;
    *(int *)((int)this + 0x10) = *(int *)((int)this + 0x10) + 4;
  }
  if (param_1 == 4) {
    uVar2 = *param_2;
    puVar4 = (uint *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                            (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    *puVar4 = uVar2;
  }
  else {
    uVar2 = param_1;
    puVar4 = (uint *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                            (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    FUN_00543e20(puVar4,param_2,uVar2);
  }
  *(uint *)((int)this + 0x10) = *(int *)((int)this + 0x10) + param_1;
  return 0;
}

