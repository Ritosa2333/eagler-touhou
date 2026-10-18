/* Entry: 0x0046a3b0; symbol: FUN_0046a3b0; body bytes: 98 */

char * __cdecl FUN_0046a3b0(char *param_1,undefined4 param_2,char *param_3)

{
  char *local_10;
  char *local_c;
  char *local_8;
  
  std::_Adl_verify_range<char*,char_const*>(&param_1,(char **)&param_2);
  local_8 = (char *)FUN_0040b2a0(&param_2);
  local_c = (char *)FUN_0040b2a0(&param_1);
  local_10 = FUN_004683b0(local_c,local_8,param_3);
  FUN_004185e0(&param_1,&local_10);
  return param_1;
}

