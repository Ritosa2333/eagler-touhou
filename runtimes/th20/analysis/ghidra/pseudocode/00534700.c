/* Entry: 0x00534700; symbol: FUN_00534700; body bytes: 45 */

void __thiscall FUN_00534700(void *this,uint param_1)

{
  *(uint *)((int)this + 0x1255c) = *(uint *)((int)this + 0x1255c) & 0xfffffff7 | (param_1 & 1) << 3;
  return;
}

