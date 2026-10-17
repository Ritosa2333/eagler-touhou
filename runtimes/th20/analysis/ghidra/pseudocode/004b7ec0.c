/* Entry: 0x004b7ec0; symbol: FUN_004b7ec0; body bytes: 126 */

void __thiscall FUN_004b7ec0(void *this,int *param_1,int *param_2)

{
  bool bVar1;
  undefined3 extraout_var;
  
  bVar1 = FUN_004885e0((int)this);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    *param_1 = ((*(int *)((int)this + 0xa8) / 100) % 1000 + 0x3a6) % 1000;
    *param_2 = (*(int *)((int)this + 0xa8) % 100 + 0x43) % 100;
  }
  else {
    *param_1 = 999;
    *param_2 = 99;
  }
  return;
}

