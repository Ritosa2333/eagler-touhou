/* Entry: 0x00413ec0; symbol: FUN_00413ec0; body bytes: 132 */

undefined4 * __thiscall
FUN_00413ec0(void *this,undefined4 param_1,undefined4 *param_2,undefined4 *param_3,
            undefined4 *param_4,undefined4 *param_5,int param_6)

{
  *(undefined4 *)this = param_1;
  *(undefined4 *)((int)this + 4) = *param_2;
  *(undefined4 *)((int)this + 8) = *param_3;
  *(undefined4 *)((int)this + 0xc) = param_3[1];
  *(undefined4 *)((int)this + 0x10) = param_3[2];
  *(undefined4 *)((int)this + 0x14) = param_3[3];
  *(undefined4 *)((int)this + 0x18) = *param_4;
  *(undefined4 *)((int)this + 0x1c) = param_4[1];
  *(undefined4 *)((int)this + 0x20) = param_4[2];
  *(undefined4 *)((int)this + 0x24) = param_4[3];
  *(undefined4 *)((int)this + 0x28) = *param_5;
  function<>((void *)((int)this + 0x30),param_6);
  return (undefined4 *)this;
}

