/* Entry: 0x004395d0; symbol: FUN_004395d0; body bytes: 100 */

void __thiscall
FUN_004395d0(void *this,undefined4 param_1,undefined4 param_2,undefined4 *param_3,
            undefined4 *param_4)

{
  FUN_00438e70(this,param_1);
  FUN_004394f0(this,param_2);
  FUN_004393f0(this,param_3);
  FUN_00438a90(this,param_4);
  *(undefined4 *)((int)this + 0x30) = *param_3;
  *(undefined4 *)((int)this + 0x34) = param_3[1];
  *(undefined4 *)((int)this + 0x38) = param_3[2];
  FUN_00423520((void *)((int)this + 0x3c),0);
  return;
}

