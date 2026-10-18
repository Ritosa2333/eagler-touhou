/* Entry: 0x00437840; symbol: FUN_00437840; body bytes: 226 */

float * __thiscall FUN_00437840(void *this,float *param_1,int param_2,int param_3)

{
  float10 fVar1;
  
  if ((*(int *)((int)this + 0x558) != 0) && ((*(uint *)((int)this + 0x49c) >> 0xc & 1) == 0)) {
    FUN_00437840(*(void **)((int)this + 0x558),param_1,*(uint *)((int)this + 0x49c) >> 5 & 1,
                 *(uint *)((int)this + 0x49c) >> 0x16 & 1);
  }
  if (param_2 != 0) {
    fVar1 = FUN_00437930((int)this);
    FUN_00458fa0(param_1,param_1,param_1,(float)fVar1);
  }
  if (param_3 != 0) {
    fVar1 = FUN_004379c0((int)this);
    *param_1 = *param_1 * (float)fVar1;
    fVar1 = FUN_004379a0((int)this);
    param_1[1] = param_1[1] * (float)fVar1;
  }
  return param_1;
}

