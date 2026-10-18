/* Entry: 0x0054f9a3; symbol: FUN_0054f9a3; body bytes: 53 */

/* WARNING: Removing unreachable block (ram,0x0054f9c0) */

bool __thiscall FUN_0054f9a3(void *this,ushort param_1)

{
  int iVar1;
  
  iVar1 = FUN_0054e9a0((int *)this);
  return (*(byte *)(iVar1 + (uint)(param_1 >> 3)) & (byte)(1 << ((byte)param_1 & 7))) != 0;
}

