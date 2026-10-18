/* Entry: 0x004f3c30; symbol: FUN_004f3c30; body bytes: 43 */

undefined4 * __thiscall FUN_004f3c30(void *this,undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
  uVar1 = param_1[1];
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = uVar1;
  *(undefined4 *)((int)this + 8) = *param_2;
  return (undefined4 *)this;
}

