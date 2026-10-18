/* Entry: 0x004a55d0; symbol: FUN_004a55d0; body bytes: 100 */

void __thiscall FUN_004a55d0(void *this,int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 0x120) == param_1 + 0x74) {
    uVar1 = FUN_0040ff90(param_1 + 0x74);
    *(undefined4 *)((int)this + 0x120) = uVar1;
  }
  FUN_00411ce0(param_1 + 0x74);
  if ((*(uint *)(param_1 + 0x354) >> 0x12 & 1) == 0) {
    *(int *)((int)this + 0x124) = *(int *)((int)this + 0x124) + -1;
  }
  return;
}

