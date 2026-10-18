/* Entry: 0x004252c0; symbol: FUN_004252c0; body bytes: 92 */

void __thiscall FUN_004252c0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_1);
  uVar2 = move<>(*(undefined4 *)((int)this + 4));
  FUN_004258c0(*(undefined4 *)((int)this + 8),uVar2,uVar1);
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + 0x10;
  return;
}

