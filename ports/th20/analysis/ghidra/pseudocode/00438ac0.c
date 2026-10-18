/* Entry: 0x00438ac0; symbol: FUN_00438ac0; body bytes: 30 */

void __thiscall FUN_00438ac0(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)((int)this + 8) = *param_1;
  *(undefined4 *)((int)this + 0xc) = uVar1;
  return;
}

