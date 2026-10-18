/* Entry: 0x004671a0; symbol: FUN_004671a0; body bytes: 129 */

void __cdecl FUN_004671a0(undefined4 param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  int local_10;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_2;
  if (param_2 != 0) {
    local_c = (undefined4 *)FUN_0040b2a0(&param_1);
    FUN_00411170();
    for (; local_10 != 0; local_10 = local_10 + -1) {
      uVar1 = param_3[1];
      *local_c = *param_3;
      local_c[1] = uVar1;
      local_c = local_c + 2;
    }
    FUN_004185e0(&param_1,&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

