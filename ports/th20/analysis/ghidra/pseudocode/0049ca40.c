/* Entry: 0x0049ca40; symbol: FUN_0049ca40; body bytes: 40 */

undefined4 __thiscall FUN_0049ca40(void *this,undefined4 param_1)

{
  *(uint *)((int)this + 0x60) = *(uint *)((int)this + 0x60) & 0xfffffffe;
  *(undefined4 *)((int)this + 0x5c) = param_1;
  return param_1;
}

