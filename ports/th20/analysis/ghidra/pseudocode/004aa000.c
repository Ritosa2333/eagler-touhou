/* Entry: 0x004aa000; symbol: FUN_004aa000; body bytes: 75 */

void __thiscall FUN_004aa000(void *this,int param_1,int param_2)

{
  int iVar1;
  
  if (0 < *(int *)((int)this + 0x38)) {
    iVar1 = FUN_004a3ee0(this,param_1);
    if (iVar1 < param_2) {
      FUN_0045c300(this,param_1);
    }
    else {
      FUN_00423520(this,param_2);
    }
  }
  return;
}

