/* Entry: 0x00477820; symbol: FUN_00477820; body bytes: 30 */

void __thiscall FUN_00477820(void *this,undefined4 *param_1)

{
  int iVar1;
  
  for (iVar1 = 7; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined4 *)this = *param_1;
    param_1 = param_1 + 1;
    this = (undefined4 *)((int)this + 4);
  }
  return;
}

