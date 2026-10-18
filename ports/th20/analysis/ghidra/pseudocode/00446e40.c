/* Entry: 0x00446e40; symbol: FUN_00446e40; body bytes: 88 */

void * __cdecl FUN_00446e40(void *param_1,undefined4 *param_2,undefined4 *param_3)

{
  longlong lVar1;
  longlong local_1c;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  local_c = *param_2;
  local_8 = param_2[1];
  local_14 = *param_3;
  local_10 = param_3[1];
  lVar1 = FUN_0044c090((undefined8 *)&local_c);
  local_1c = FUN_0044c090((undefined8 *)&local_14);
  local_1c = lVar1 - local_1c;
  FUN_00446da0(param_1,(undefined4 *)&local_1c);
  return param_1;
}

