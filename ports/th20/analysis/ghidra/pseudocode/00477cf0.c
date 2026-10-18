/* Entry: 0x00477cf0; symbol: FUN_00477cf0; body bytes: 64 */

undefined4 __thiscall FUN_00477cf0(void *this,undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  
  if (*(int *)((int)this + 0x14) != 0) {
    pcVar1 = *(code **)(**(int **)((int)this + 0x14) + 0x14);
    (*pcVar1)(param_1,param_2,pcVar1,*(undefined4 *)((int)this + 0x14));
  }
  return 0;
}

