/* Entry: 0x004e9b30; symbol: FUN_004e9b30; body bytes: 240 */

void __cdecl
FUN_004e9b30(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            int *param_5)

{
  undefined4 *puVar1;
  undefined1 local_44 [12];
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  undefined4 *local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  if ((param_5[1] < 0) && (*param_5 < 1)) {
    local_38 = param_3;
    local_34 = param_4;
    puVar1 = (undefined4 *)move<>(&param_2);
    local_20 = *puVar1;
    FUN_004e9af0(param_1);
  }
  else {
    local_18 = param_5[1];
    local_1c = FUN_004ed640(param_3,param_4,&local_18);
    local_28 = FUN_004f3c30(local_44,&param_3,&local_1c);
    local_2c = local_18;
    puVar1 = (undefined4 *)move<>(&param_2);
    local_30 = *puVar1;
    local_24 = local_30;
    FUN_004efec0(param_1,local_30,local_2c,param_5,'\x01',local_28);
  }
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

