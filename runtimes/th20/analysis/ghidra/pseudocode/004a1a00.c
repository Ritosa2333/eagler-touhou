/* Entry: 0x004a1a00; symbol: FUN_004a1a00; body bytes: 94 */

void __thiscall FUN_004a1a00(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_1);
  uVar2 = move<>(*(undefined4 *)((int)this + 4));
  FUN_004a2640(*(undefined4 *)((int)this + 8),uVar2,uVar1);
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + 0x88;
  return;
}

