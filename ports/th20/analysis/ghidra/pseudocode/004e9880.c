/* Entry: 0x004e9880; symbol: FUN_004e9880; body bytes: 107 */

undefined4 * __cdecl
FUN_004e9880(undefined4 *param_1,undefined4 param_2,char *param_3,int *param_4,undefined4 param_5)

{
  undefined4 *puVar1;
  undefined1 local_28 [8];
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_14;
  
  puVar1 = FUN_0040ddd0(local_28,param_3);
  local_20 = *puVar1;
  local_1c = puVar1[1];
  puVar1 = (undefined4 *)move<>(&param_2);
  local_14 = *puVar1;
  FUN_004e9b30(param_1,local_14,local_20,local_1c,param_4);
  return param_1;
}

