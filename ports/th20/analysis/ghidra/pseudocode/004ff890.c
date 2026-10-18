/* Entry: 0x004ff890; symbol: FUN_004ff890; body bytes: 92 */

char * __fastcall FUN_004ff890(char **param_1)

{
  char *pcVar1;
  char cVar2;
  char **in_stack_fffffff4;
  
  if ((int)param_1[3] < 0) {
    func_0x004fcd20("Can not switch from manual to automatic indexing");
  }
  cVar2 = FUN_00411170();
  if ((cVar2 != '\0') && (param_1[2] <= param_1[3])) {
    std::_Adl_verify_range<char*,char_const*>(in_stack_fffffff4,param_1);
  }
  pcVar1 = param_1[3];
  param_1[3] = param_1[3] + 1;
  return pcVar1;
}

