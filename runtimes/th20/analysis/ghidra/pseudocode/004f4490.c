/* Entry: 0x004f4490; symbol: FUN_004f4490; body bytes: 63 */

undefined4 * __thiscall
FUN_004f4490(void *this,undefined4 param_1,undefined4 *param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)move<>(param_1);
  *(undefined4 *)this = *puVar2;
  uVar1 = param_2[1];
  *(undefined4 *)((int)this + 4) = *param_2;
  *(undefined4 *)((int)this + 8) = uVar1;
  *(undefined4 *)((int)this + 0xc) = *param_3;
  return (undefined4 *)this;
}

