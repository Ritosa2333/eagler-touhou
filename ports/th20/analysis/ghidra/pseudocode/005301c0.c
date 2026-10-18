/* Entry: 0x005301c0; symbol: FUN_005301c0; body bytes: 370 */

int __thiscall FUN_005301c0(void *this,undefined4 param_1,int param_2)

{
  int iVar1;
  
  switch(param_1) {
  case 1:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0x124 + param_2 * 8;
    break;
  case 2:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 300 + param_2 * 8;
    break;
  case 3:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0x13c + param_2 * 8;
    break;
  case 4:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0x154 + param_2 * 8;
    break;
  default:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0x124;
  }
  return iVar1;
}

