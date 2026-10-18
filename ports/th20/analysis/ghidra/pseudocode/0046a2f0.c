/* Entry: 0x0046a2f0; symbol: FUN_0046a2f0; body bytes: 187 */

void __cdecl FUN_0046a2f0(undefined4 param_1,size_t param_2,byte *param_3)

{
  char cVar1;
  byte *local_14;
  size_t local_10;
  byte *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_2;
  if (param_2 != 0) {
    local_c = (byte *)FUN_0040b2a0(&param_1);
    cVar1 = FUN_00411170();
    if (cVar1 == '\0') {
      FUN_00468380(local_c,*param_3,local_10);
      local_14 = local_c + local_10;
      FUN_004185e0(&param_1,&local_14);
    }
    else {
      for (; local_10 != 0; local_10 = local_10 - 1) {
        *local_c = *param_3;
        local_c = local_c + 1;
      }
      FUN_004185e0(&param_1,&local_c);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

