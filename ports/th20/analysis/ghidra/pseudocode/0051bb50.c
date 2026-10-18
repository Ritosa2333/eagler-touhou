/* Entry: 0x0051bb50; symbol: FUN_0051bb50; body bytes: 57 */

bool __thiscall FUN_0051bb50(void *this,ushort param_1,byte param_2)

{
  return (*(ushort *)(*(int *)((int)this + 0xc) + (uint)param_2 * 2) & param_1) != 0;
}

