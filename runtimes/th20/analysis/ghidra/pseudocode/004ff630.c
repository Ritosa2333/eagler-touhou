/* Entry: 0x004ff630; symbol: FUN_004ff630; body bytes: 52 */

void __thiscall FUN_004ff630(void *this,undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  
  pcVar1 = *(code **)(**(int **)((int)this + 0x2c) + 0x14);
  (*pcVar1)(param_1,param_2,pcVar1,*(undefined4 *)((int)this + 0x2c));
  return;
}

