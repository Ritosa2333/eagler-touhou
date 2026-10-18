/* Entry: 0x004e1850; symbol: FUN_004e1850; body bytes: 108 */

void __thiscall FUN_004e1850(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 8) == *(int *)((int)this + 0xc)) {
    uVar1 = move<>(param_1);
    FUN_004e18c0(this,*(int *)((int)this + 8),uVar1);
  }
  else {
    uVar1 = move<>(param_1);
    FUN_004e17c0(this,uVar1);
  }
  return;
}

