/* Entry: 0x004379e0; symbol: FUN_004379e0; body bytes: 660 */

void __thiscall FUN_004379e0(void *this,float *param_1)

{
  int iVar1;
  float10 fVar2;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(char *)((int)this + 0x4a4) == '\x01') || (*(char *)((int)this + 0x4a4) == '\x03')) {
    fVar2 = FUN_00438290(0x5b6758);
    FUN_00429690(param_1,(float)fVar2);
  }
  else if ((*(char *)((int)this + 0x4a4) == '\x02') || (*(char *)((int)this + 0x4a4) == '\x04')) {
    fVar2 = FUN_00438290(0x5b6758);
    FUN_00429690(param_1,(float)fVar2 * DAT_0056e0ec);
  }
  if ((*(int *)((int)this + 0x558) == 0) || ((*(uint *)((int)this + 0x49c) >> 0xc & 1) != 0)) {
    if ((*(byte *)((int)this + 0x4a3) & 3) != 0) {
      if ((*(byte *)((int)this + 0x4a3) & 3) == 1) {
        iVar1 = FUN_00437660(0x5b6758);
        *param_1 = *param_1 + (float)iVar1;
        iVar1 = FUN_004376a0(0x5b6758);
        param_1[1] = param_1[1] + (float)iVar1;
      }
      else {
        iVar1 = FUN_00437680(0x5b6758);
        *param_1 = *param_1 + (float)iVar1;
        iVar1 = FUN_004376c0(0x5b6758);
        param_1[1] = param_1[1] + (float)iVar1;
      }
    }
  }
  else {
    if ((*(uint *)((int)this + 0x49c) >> 5 & 1) != 0) {
      fVar2 = FUN_00437930(*(int *)((int)this + 0x558));
      FUN_00458fa0(param_1,param_1,param_1,(float)fVar2);
    }
    if ((*(uint *)((int)this + 0x49c) >> 0x16 & 1) != 0) {
      fVar2 = FUN_004379c0(*(int *)((int)this + 0x558));
      *param_1 = *param_1 * (float)fVar2;
      fVar2 = FUN_004379a0(*(int *)((int)this + 0x558));
      param_1[1] = param_1[1] * (float)fVar2;
    }
    FUN_00422e10(local_14);
    FUN_004376e0(*(void **)((int)this + 0x558),local_14);
    FUN_004296e0(param_1,local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

