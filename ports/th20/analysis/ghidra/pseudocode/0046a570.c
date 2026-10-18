/* Entry: 0x0046a570; symbol: FUN_0046a570; body bytes: 48 */

undefined4 * __thiscall
FUN_0046a570(void *this,undefined4 *param_1,undefined4 *param_2,undefined4 *param_3)

{
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = *param_2;
  *(undefined4 *)((int)this + 8) = *param_3;
  return (undefined4 *)this;
}

