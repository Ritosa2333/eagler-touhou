/* Entry: 0x004b6580; symbol: FUN_004b6580; body bytes: 72 */

void __thiscall FUN_004b6580(void *this,uint param_1)

{
  uint uVar1;
  
  uVar1 = FUN_004b6d80(this,param_1);
  move<>(*(int *)(*(int *)((int)this + 4) + uVar1 * 4) + (param_1 % 4) * 4);
  return;
}

