/* Entry: 0x0045d360; symbol: FUN_0045d360; body bytes: 202 */

void __cdecl FUN_0045d360(undefined4 param_1,int param_2,uint *param_3)

{
  char cVar1;
  uint *local_14;
  int local_10;
  uint *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_2;
  if (param_2 != 0) {
    local_c = (uint *)FUN_0040b2a0(&param_1);
    cVar1 = FUN_00411170();
    if ((cVar1 == '\0') && (cVar1 = FUN_0045d270(param_3), cVar1 != '\0')) {
      FUN_00425320(local_c,local_10);
      local_14 = local_c + local_10;
      FUN_004185e0(&param_1,&local_14);
    }
    else {
      for (; local_10 != 0; local_10 = local_10 + -1) {
        *local_c = *param_3;
        local_c = local_c + 1;
      }
      FUN_004185e0(&param_1,&local_c);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

