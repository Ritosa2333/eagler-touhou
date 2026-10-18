/* Entry: 0x0051c0d0; symbol: FUN_0051c0d0; body bytes: 51 */

void __thiscall FUN_0051c0d0(void *this,int param_1)

{
  **(int **)((int)this + 0x30) = **(int **)((int)this + 0x30) - param_1;
  **(int **)((int)this + 0x20) = **(int **)((int)this + 0x20) + param_1;
  return;
}

