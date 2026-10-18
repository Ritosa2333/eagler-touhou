/* Entry: 0x0047acb0; symbol: FUN_0047acb0; body bytes: 92 */

void __thiscall FUN_0047acb0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_1);
  uVar2 = move<>(*(undefined4 *)((int)this + 4));
  FUN_0047b4a0(*(undefined4 *)((int)this + 8),uVar2,uVar1);
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + 0x2c;
  return;
}

