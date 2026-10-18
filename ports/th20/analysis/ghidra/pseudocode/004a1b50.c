/* Entry: 0x004a1b50; symbol: FUN_004a1b50; body bytes: 108 */

void __thiscall FUN_004a1b50(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 8) == *(int *)((int)this + 0xc)) {
    uVar1 = move<>(param_1);
    FUN_004a1bc0(this,*(int *)((int)this + 8),uVar1);
  }
  else {
    uVar1 = move<>(param_1);
    FUN_004a1ac0(this,uVar1);
  }
  return;
}

