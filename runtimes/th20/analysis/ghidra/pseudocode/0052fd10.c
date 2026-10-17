/* Entry: 0x0052fd10; symbol: FUN_0052fd10; body bytes: 85 */

void __thiscall FUN_0052fd10(void *this,int param_1,int param_2,int param_3)

{
  char cVar1;
  int iVar2;
  void *this_00;
  
  cVar1 = FUN_00530520();
  if (cVar1 == '\0') {
    iVar2 = FUN_00460830(0);
    this_00 = (void *)FUN_004ff6b0(iVar2);
    FUN_00504d40(this_00,param_1,param_2,*(int *)((int)this + 4) * 0x14 + 10 + param_3);
  }
  return;
}

