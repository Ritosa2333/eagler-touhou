/* Entry: 0x0041f760; symbol: FUN_0041f760; body bytes: 81 */

void FUN_0041f760(void *param_1,undefined4 param_2,align_val_t param_3)

{
  undefined4 uVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,1);
  FUN_0040b730(&local_c,uVar1);
  operator_delete(param_1,param_3);
  FUN_0040b900(&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

