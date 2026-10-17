/* Entry: 0x00446da0; symbol: FUN_00446da0; body bytes: 32 */

undefined4 * __thiscall FUN_00446da0(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = uVar1;
  return (undefined4 *)this;
}

