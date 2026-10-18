/* Entry: 0x004ff760; symbol: FUN_004ff760; body bytes: 52 */

void __thiscall FUN_004ff760(void *this,undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  
  pcVar1 = *(code **)(**(int **)((int)this + 0x30) + 0x18);
  (*pcVar1)(param_1,param_2,pcVar1,*(undefined4 *)((int)this + 0x30));
  return;
}

