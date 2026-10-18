/* Entry: 0x0040c240; symbol: FUN_0040c240; body bytes: 54 */

int __thiscall FUN_0040c240(void *this,int param_1,char **param_2)

{
  int iVar1;
  int *piVar2;
  char **unaff_ESI;
  
  std::_Adl_verify_range<char*,char_const*>(param_2,unaff_ESI);
  piVar2 = (int *)move<>(this);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + param_1;
  UNLOCK();
  return iVar1;
}

