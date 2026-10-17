/* Entry: 0x004f3350; symbol: FUN_004f3350; body bytes: 127 */

void __cdecl FUN_004f3350(undefined4 param_1,undefined4 param_2,byte *param_3)

{
  char cVar1;
  byte *pbVar2;
  byte *local_8;
  
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  local_8 = (byte *)FUN_0040b2a0(&param_1);
  pbVar2 = (byte *)FUN_0040b2a0(&param_2);
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    FUN_00468380(local_8,*param_3,(int)pbVar2 - (int)local_8);
  }
  else {
    for (; local_8 != pbVar2; local_8 = local_8 + 1) {
      *local_8 = *param_3;
    }
  }
  return;
}

