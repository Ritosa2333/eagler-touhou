/* Entry: 0x0045d430; symbol: FUN_0045d430; body bytes: 123 */

void __cdecl FUN_0045d430(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int local_10;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_2;
  if (param_2 != 0) {
    local_c = (undefined4 *)FUN_0040b2a0(&param_1);
    FUN_00411170();
    for (; local_10 != 0; local_10 = local_10 + -1) {
      *local_c = *param_3;
      local_c = local_c + 1;
    }
    FUN_004185e0(&param_1,&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

