/* Entry: 0x004478b0; symbol: FUN_004478b0; body bytes: 98 */

char * __cdecl FUN_004478b0(char *param_1,undefined4 param_2,short *param_3)

{
  short *local_10;
  short *local_c;
  short *local_8;
  
  std::_Adl_verify_range<char*,char_const*>(&param_1,(char **)&param_2);
  local_8 = (short *)FUN_0040b2a0(&param_2);
  local_c = (short *)FUN_0040b2a0(&param_1);
  local_10 = FUN_00446f90(local_c,local_8,param_3);
  FUN_004185e0(&param_1,&local_10);
  return param_1;
}

