/* Entry: 0x00467f50; symbol: FUN_00467f50; body bytes: 72 */

char ** __fastcall FUN_00467f50(char **param_1)

{
  char *pcVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)move<>(&stack0x00000004);
  pcVar1 = (char *)puVar2[1];
  *param_1 = (char *)*puVar2;
  param_1[1] = pcVar1;
  move<>(&stack0x0000000c);
  std::_Adl_verify_range<char*,char_const*>(param_1,param_1 + 2);
  return param_1;
}

