/* Entry: 0x0053a170; symbol: FUN_0053a170; body bytes: 151 */

void __fastcall FUN_0053a170(undefined4 *param_1)

{
  if (param_1[2] != 0) {
    std::_Adl_verify_range<char*,char_const*>
              ((char **)"info : %s close arcfile\r\n",(char **)param_1[2]);
    if (param_1[2] != 0) {
      FUN_0041f670((LPVOID)param_1[2]);
      param_1[2] = 0;
    }
  }
  param_1[2] = 0;
  FUN_00539ab0((NumaInformation *)*param_1);
  *param_1 = 0;
  FUN_00539b20((int *)param_1[3]);
  param_1[3] = 0;
  param_1[1] = 0;
  return;
}

