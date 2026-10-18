/* Entry: 0x00426050; symbol: FUN_00426050; body bytes: 81 */

void __fastcall FUN_00426050(undefined4 *param_1)

{
  if (param_1[1] != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)param_1[3],(char **)param_1[4]);
    FUN_00428e80((void *)*param_1,param_1[1],param_1[2]);
  }
  return;
}

