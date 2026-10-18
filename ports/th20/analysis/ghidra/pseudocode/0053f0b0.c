/* Entry: 0x0053f0b0; symbol: FUN_0053f0b0; body bytes: 242 */

undefined4 __thiscall FUN_0053f0b0(void *this,uint param_1,float *param_2,char param_3)

{
  float *pfVar1;
  uint *puVar2;
  int *piVar3;
  
  *(uint *)((int)this + 0x10) = *(int *)((int)this + 0x10) - param_1;
  if (param_1 == 4) {
    pfVar1 = (float *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                             (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    *param_2 = *pfVar1;
  }
  else {
    puVar2 = (uint *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                            (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    FUN_00543e20((uint *)param_2,puVar2,param_1);
  }
  if (param_3 != '\0') {
    *(int *)((int)this + 0x10) = *(int *)((int)this + 0x10) + -4;
    piVar3 = (int *)FUN_0053b530(this,(int)(*(int *)((int)this + 0x10) +
                                           (*(int *)((int)this + 0x10) >> 0x1f & 3U)) >> 2);
    if (*piVar3 == 0x66) {
      if (param_3 == 'i') {
        *param_2 = (float)(int)*param_2;
      }
    }
    else if ((*piVar3 == 0x69) && (param_3 == 'f')) {
      *param_2 = (float)(int)*param_2;
    }
  }
  return 0;
}

