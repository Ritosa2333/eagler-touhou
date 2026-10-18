/* Entry: 0x00445a00; symbol: FUN_00445a00; body bytes: 60 */

undefined4 __thiscall FUN_00445a00(void *this,uint param_1)

{
  int *piVar1;
  
  piVar1 = (int *)FUN_0043eb10((void *)((int)this + 0x6000730),(int)param_1 >> 8);
  return *(undefined4 *)(*(int *)(*piVar1 + 0x58) + (param_1 & 0xff) * 0x18);
}

