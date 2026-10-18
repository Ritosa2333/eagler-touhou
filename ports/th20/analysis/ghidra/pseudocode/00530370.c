/* Entry: 0x00530370; symbol: FUN_00530370; body bytes: 370 */

int __thiscall FUN_00530370(void *this,undefined4 param_1,int param_2)

{
  int iVar1;
  
  switch(param_1) {
  case 1:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + 0xd4 + *(int *)((int)this + 4) * 0xa0 + param_2 * 8;
    break;
  case 2:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0xdc + param_2 * 8;
    break;
  case 3:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0xec + param_2 * 8;
    break;
  case 4:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + *(int *)((int)this + 4) * 0xa0 + 0x104 + param_2 * 8;
    break;
  default:
    iVar1 = FUN_00460830(0);
    iVar1 = FUN_00506340(iVar1);
    iVar1 = iVar1 + 0xd4 + *(int *)((int)this + 4) * 0xa0;
  }
  return iVar1;
}

