/* Entry: 0x0051ca70; symbol: FUN_0051ca70; body bytes: 49 */

void __thiscall FUN_0051ca70(void *this,undefined4 param_1,int param_2,int param_3)

{
  **(undefined4 **)((int)this + 0x10) = param_1;
  **(int **)((int)this + 0x20) = param_2;
  **(int **)((int)this + 0x30) = param_3 - param_2;
  return;
}

