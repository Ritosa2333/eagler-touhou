/* Entry: 0x00537a30; symbol: FUN_00537a30; body bytes: 98 */

void __thiscall FUN_00537a30(void *this,int param_1,int param_2,int param_3)

{
  char cVar1;
  int iVar2;
  void *this_00;
  
  cVar1 = FUN_00530520();
  if (cVar1 == '\0') {
    iVar2 = FUN_00460830(0);
    this_00 = (void *)FUN_004ff6b0(iVar2);
    FUN_00504d40(this_00,param_1,param_2,*(int *)((int)this + 4) * 0x14 + 5 + param_3);
  }
  FUN_00537aa0(this,param_3);
  return;
}

