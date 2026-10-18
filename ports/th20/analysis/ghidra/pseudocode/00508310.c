/* Entry: 0x00508310; symbol: FUN_00508310; body bytes: 80 */

undefined4 __thiscall FUN_00508310(void *this,int param_1)

{
  undefined4 local_c;
  
  FUN_0050a990((longlong *)(*(int *)((int)this + 0x1c) + 0x10));
  if (param_1 == 0) {
    local_c = FUN_00474d60(0x5ba568);
  }
  else {
    local_c = param_1 + 7;
  }
  *(int *)(*(int *)((int)this + 0x1c) + 0xf4) = local_c;
  return 0;
}

