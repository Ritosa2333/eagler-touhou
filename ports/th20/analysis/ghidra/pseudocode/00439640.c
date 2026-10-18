/* Entry: 0x00439640; symbol: FUN_00439640; body bytes: 92 */

void __thiscall
FUN_00439640(void *this,undefined4 param_1,undefined4 param_2,undefined4 *param_3,
            undefined4 *param_4)

{
  undefined4 uVar1;
  
  FUN_00438e90(this,param_1);
  FUN_00439510(this,param_2);
  FUN_00439420(this,param_3);
  FUN_00438ac0(this,param_4);
  uVar1 = param_3[1];
  *(undefined4 *)((int)this + 0x20) = *param_3;
  *(undefined4 *)((int)this + 0x24) = uVar1;
  FUN_00423520((void *)((int)this + 0x28),0);
  return;
}

