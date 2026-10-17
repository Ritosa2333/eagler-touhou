/* Entry: 0x0045ac90; symbol: FUN_0045ac90; body bytes: 92 */

undefined4 __thiscall
FUN_0045ac90(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 uVar1;
  
  *(undefined4 *)((int)this + 0x90) = param_3;
  *(undefined4 *)((int)this + 0x88) = param_2;
  *(undefined4 *)((int)this + 0x80) = param_1;
  *(undefined4 *)((int)this + 0x84) = *(undefined4 *)((int)this + 0x80);
  *(undefined4 *)((int)this + 0x7c) = 1;
  if (param_4 == 1) {
    uVar1 = 0;
  }
  else {
    uVar1 = 0x80004001;
  }
  return uVar1;
}

