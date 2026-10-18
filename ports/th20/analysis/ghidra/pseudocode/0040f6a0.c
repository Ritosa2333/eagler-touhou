/* Entry: 0x0040f6a0; symbol: FUN_0040f6a0; body bytes: 72 */

void __fastcall FUN_0040f6a0(char **param_1)

{
  char cVar1;
  undefined4 local_8;
  
  std::_Adl_verify_range<char*,char_const*>(param_1,param_1);
  cVar1 = FUN_00411170();
  if (cVar1 != '\0') {
    for (local_8 = 0; local_8 < 0x10; local_8 = local_8 + 1) {
      *(undefined1 *)((int)param_1 + local_8) = 0;
    }
  }
  return;
}

