/* Entry: 0x00439550; symbol: FUN_00439550; body bytes: 86 */

void __thiscall
FUN_00439550(void *this,undefined4 param_1,undefined4 param_2,undefined4 *param_3,
            undefined4 *param_4)

{
  FUN_0041df50(this,param_1);
  FUN_004394d0(this,param_2);
  FUN_004393d0(this,param_3);
  FUN_00438a70(this,param_4);
  *(undefined4 *)((int)this + 0x10) = *param_3;
  FUN_00423520((void *)((int)this + 0x14),0);
  return;
}

