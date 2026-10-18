/* Entry: 0x0042b590; symbol: FUN_0042b590; body bytes: 63 */

undefined4 __thiscall FUN_0042b590(void *this,SIZE_T param_1)

{
  undefined4 uVar1;
  
  *(SIZE_T *)((int)this + 0x56c) = param_1;
  uVar1 = FUN_0041f610(param_1);
  *(undefined4 *)((int)this + 0x564) = uVar1;
  return *(undefined4 *)((int)this + 0x564);
}

