/* Entry: 0x0049cce0; symbol: FUN_0049cce0; body bytes: 130 */

void __cdecl FUN_0049cce0(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int local_10;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_2;
  if (param_2 != 0) {
    local_c = (undefined4 *)FUN_0040b2a0(&param_1);
    FUN_00411170();
    for (; local_10 != 0; local_10 = local_10 + -1) {
      puVar2 = param_3;
      puVar3 = local_c;
      for (iVar1 = 0x12; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar3 = puVar3 + 1;
      }
      local_c = local_c + 0x12;
    }
    FUN_004185e0(&param_1,&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

