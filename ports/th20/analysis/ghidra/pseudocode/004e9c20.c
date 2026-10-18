/* Entry: 0x004e9c20; symbol: FUN_004e9c20; body bytes: 66 */

undefined4 * __cdecl FUN_004e9c20(undefined4 *param_1,undefined4 param_2,char param_3)

{
  undefined4 *puVar1;
  char *local_8;
  
  if (param_3 == '\0') {
    local_8 = "false";
  }
  else {
    local_8 = "true";
  }
  puVar1 = (undefined4 *)move<>(&param_2);
  FUN_004e9810(param_1,*puVar1,local_8);
  return param_1;
}

