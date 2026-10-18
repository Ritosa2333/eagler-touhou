/* Entry: 0x004388f0; symbol: FUN_004388f0; body bytes: 79 */

void __thiscall FUN_004388f0(void *this,undefined4 param_1)

{
  code *pcVar1;
  
  if (*(int *)((int)this + 0x568) != 0) {
    pcVar1 = *(code **)(**(int **)((int)this + 0x568) + 0x10);
    (*pcVar1)(param_1,pcVar1,*(undefined4 *)((int)this + 0x568));
  }
  *(undefined4 *)((int)this + 0x438) = param_1;
  return;
}

