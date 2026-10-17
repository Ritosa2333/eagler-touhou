/* Entry: 0x0044eb30; symbol: FUN_0044eb30; body bytes: 182 */

int __thiscall FUN_0044eb30(void *this,uint param_1,char **param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_00437970(this,param_1);
  if (iVar1 == 0) {
    iVar1 = FUN_0044ed40(this,param_1,param_2);
    if (iVar1 == 0) {
      iVar1 = 0;
    }
    else {
      FUN_004493c0((void *)(iVar1 + 0x5c),1);
      while ((iVar2 = FUN_004292c0(iVar1 + 0x5c), iVar2 != 0 && ((DAT_005c5888 >> 5 & 3) == 0))) {
        Sleep(1);
      }
      std::_Adl_verify_range<char*,char_const*>((char **)"::preloadAnimEnd : %s\n",param_2);
    }
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)"::preloadAnim already : %s\n",param_2);
    iVar1 = FUN_00437970(this,param_1);
  }
  return iVar1;
}

