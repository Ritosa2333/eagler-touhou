/* Entry: 0x0053e630; symbol: FUN_0053e630; body bytes: 102 */

void __thiscall FUN_0053e630(void *this,int param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_0053f750((int)this);
  uVar2 = param_1 >> 0x1f;
  if (uVar1 < ((int)(param_1 + (uVar2 & 3)) >> 2) + 1U) {
    FUN_0053f370(this,((int)(param_1 + (uVar2 & 3)) >> 2) + 1);
  }
  FUN_0053b530(this,(int)(param_1 + (uVar2 & 3)) >> 2);
  return;
}

