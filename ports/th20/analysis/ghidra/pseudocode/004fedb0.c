/* Entry: 0x004fedb0; symbol: FUN_004fedb0; body bytes: 72 */

void FUN_004fedb0(char *param_1)

{
  char cVar1;
  char **unaff_EBP;
  char **extraout_var;
  char **ppcVar2;
  
  cVar1 = FUN_00411170();
  ppcVar2 = extraout_var;
  if ((cVar1 != '\0') && (extraout_var[2] <= param_1)) {
    std::_Adl_verify_range<char*,char_const*>(extraout_var,unaff_EBP);
  }
  if (0 < (int)ppcVar2[3]) {
    func_0x004fcd20("Can not switch from automatic to manual indexing");
  }
  ppcVar2[3] = (char *)0xffffffff;
  return;
}

