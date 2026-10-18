/* Entry: 0x0049ca70; symbol: FUN_0049ca70; body bytes: 44 */

undefined4 __thiscall FUN_0049ca70(void *this,int param_1,undefined4 param_2)

{
  *(uint *)((int)this + 0x60) = *(uint *)((int)this + 0x60) | 1;
  *(undefined4 *)((int)this + param_1 * 4 + 0x50) = param_2;
  return param_2;
}

