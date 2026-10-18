/* Entry: 0x00415070; symbol: FUN_00415070; body bytes: 59 */

void __thiscall FUN_00415070(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
                    /* WARNING: Load size is inaccurate */
  uVar1 = (**(code **)(*this + 4))(param_1,param_2);
  move<>(param_1,uVar1);
  return;
}

