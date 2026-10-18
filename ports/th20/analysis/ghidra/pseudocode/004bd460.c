/* Entry: 0x004bd460; symbol: FUN_004bd460; body bytes: 64 */

int __thiscall FUN_004bd460(void *this,int param_1,int param_2)

{
  int iVar1;
  
  if ((((param_1 < 0) || (1 < param_1)) || (param_2 < 0)) || (8 < param_2)) {
    iVar1 = 0;
  }
  else {
    iVar1 = (int)this + param_2 * 0x7ae8 + param_1 * 0x45228 + 0x10;
  }
  return iVar1;
}

