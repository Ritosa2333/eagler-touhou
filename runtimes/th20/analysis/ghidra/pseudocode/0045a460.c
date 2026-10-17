/* Entry: 0x0045a460; symbol: FUN_0045a460; body bytes: 53 */

undefined4 __thiscall FUN_0045a460(void *this,uint param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 8) == 0) {
    uVar1 = 0;
  }
  else if (param_1 < *(uint *)((int)this + 0x14)) {
    uVar1 = *(undefined4 *)(*(int *)((int)this + 8) + param_1 * 4);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

