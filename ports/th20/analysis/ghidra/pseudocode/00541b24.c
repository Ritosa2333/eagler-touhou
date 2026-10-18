/* Entry: 0x00541b24; symbol: FUN_00541b24; body bytes: 141 */

/* WARNING: Function: __EH_prolog3 replaced with injection: EH_prolog3 */
/* WARNING: Function: __EH_epilog3 replaced with injection: EH_epilog3 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

_Locimp * __cdecl FUN_00541b24(char param_1)

{
  code *pcVar1;
  _Locimp *p_Var2;
  _Lockit local_14 [12];
  undefined4 local_8;
  undefined4 uStack_4;
  
  uStack_4 = 4;
  local_8 = 0x541b30;
  std::_Lockit::_Lockit(local_14,0);
  local_8 = 0;
  p_Var2 = DAT_005e4e2c;
  if (DAT_005e4e2c == (_Locimp *)0x0) {
    p_Var2 = std::locale::_Locimp::_New_Locimp(false);
    std::locale::_Setgloballocale(p_Var2);
    *(undefined4 *)(p_Var2 + 0x10) = 0x3f;
    FUN_00541a86(p_Var2 + 0x18,(uint *)&DAT_0056cde0);
    pcVar1 = *(code **)(*(int *)p_Var2 + 4);
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pcVar1)();
    _DAT_005e4e04 = p_Var2;
    _DAT_005e4e20 = p_Var2;
  }
  if (param_1 != '\0') {
    pcVar1 = *(code **)(*(int *)p_Var2 + 4);
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pcVar1)();
  }
  FUN_005419ac((int *)local_14);
  return p_Var2;
}

