/* Entry: 0x00413370; symbol: FUN_00413370; body bytes: 108 */

void __thiscall FUN_00413370(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 8) == *(int *)((int)this + 0xc)) {
    uVar1 = move<>(param_1);
    FUN_004133e0(this,*(int *)((int)this + 8),uVar1);
  }
  else {
    uVar1 = move<>(param_1);
    FUN_004132e0(this,uVar1);
  }
  return;
}

