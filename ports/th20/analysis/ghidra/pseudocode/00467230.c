/* Entry: 0x00467230; symbol: FUN_00467230; body bytes: 172 */

undefined4 * __cdecl FUN_00467230(undefined4 *param_1,char *param_2,char *param_3,int param_4)

{
  undefined4 *local_14;
  int local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  std::_Adl_verify_range<char*,char_const*>(&param_2,&param_3);
  if (param_4 == 0) {
    *param_1 = param_2;
  }
  else {
    local_10 = FUN_00467070(&param_2);
    local_8 = (undefined4 *)FUN_00467070(&param_3);
    if (param_4 < (int)local_8 - local_10 >> 3) {
      local_c = local_8 + param_4 * -2;
      local_14 = FUN_00467080(local_10,(int)local_c,local_8);
      FUN_004670e0(&param_2,&local_14);
      *param_1 = param_2;
    }
    else {
      *param_1 = param_3;
    }
  }
  return param_1;
}

