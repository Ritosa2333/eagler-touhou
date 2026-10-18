/* Entry: 0x00478f70; symbol: FUN_00478f70; body bytes: 163 */

void __thiscall FUN_00478f70(void *this,void *param_1)

{
  float10 fVar1;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(int *)((int)this + 0x558) != 0) && ((*(uint *)((int)this + 0x49c) >> 0xc & 1) == 0)) {
    if ((*(uint *)((int)this + 0x49c) >> 5 & 1) != 0) {
      fVar1 = FUN_00437930(*(int *)((int)this + 0x558));
      FUN_00458fa0(param_1,(float *)param_1,(float *)param_1,(float)fVar1);
    }
    FUN_00422e10(local_14);
    FUN_00478f00(*(void **)((int)this + 0x558),local_14);
    FUN_004296e0(param_1,local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

