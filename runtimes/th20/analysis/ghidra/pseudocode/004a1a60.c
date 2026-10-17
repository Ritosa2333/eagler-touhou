/* Entry: 0x004a1a60; symbol: FUN_004a1a60; body bytes: 92 */

void __thiscall FUN_004a1a60(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_1);
  uVar2 = move<>(*(undefined4 *)((int)this + 4));
  FUN_004a26b0(*(undefined4 *)((int)this + 8),uVar2,uVar1);
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + 0x1c;
  return;
}

