/* Entry: 0x00439420; symbol: FUN_00439420; body bytes: 29 */

void __thiscall FUN_00439420(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = uVar1;
  return;
}

