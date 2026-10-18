/* Entry: 0x004f3b90; symbol: FUN_004f3b90; body bytes: 65 */

undefined4 * __thiscall
FUN_004f3b90(void *this,undefined4 *param_1,undefined4 *param_2,undefined4 *param_3,
            undefined4 *param_4)

{
  undefined4 uVar1;
  
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = *param_2;
  *(undefined4 *)((int)this + 8) = *param_3;
  uVar1 = param_4[1];
  *(undefined4 *)((int)this + 0x10) = *param_4;
  *(undefined4 *)((int)this + 0x14) = uVar1;
  return (undefined4 *)this;
}

