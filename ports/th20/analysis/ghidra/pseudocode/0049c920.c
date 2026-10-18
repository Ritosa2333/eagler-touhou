/* Entry: 0x0049c920; symbol: FUN_0049c920; body bytes: 30 */

void __thiscall FUN_0049c920(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)((int)this + 0x10) = *param_1;
  *(undefined4 *)((int)this + 0x14) = uVar1;
  return;
}

