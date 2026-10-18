/* Entry: 0x0054c39a; symbol: FUN_0054c39a; body bytes: 61 */

undefined4 * __thiscall FUN_0054c39a(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  
  *(undefined4 *)this = *param_1;
  uVar1 = param_1[3];
  *(undefined4 *)((int)this + 8) = param_1[2];
  *(undefined4 *)((int)this + 0xc) = uVar1;
  uVar1 = param_1[5];
  *(undefined4 *)((int)this + 0x10) = param_1[4];
  *(undefined4 *)((int)this + 0x14) = uVar1;
  *(undefined4 *)((int)this + 0x18) = param_1[6];
  *param_1 = 0;
  param_1[6] = 0;
  return (undefined4 *)this;
}

