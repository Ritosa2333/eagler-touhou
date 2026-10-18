/* Entry: 0x004efaa0; symbol: FUN_004efaa0; body bytes: 350 */

void __cdecl
FUN_004efaa0(undefined4 *param_1,undefined4 param_2,int param_3,int *param_4,char param_5,
            void *param_6)

{
  undefined4 *puVar1;
  undefined8 uVar2;
  undefined4 local_34;
  undefined8 local_30;
  undefined8 local_28;
  char local_20;
  int local_1c;
  int local_18;
  char local_11;
  int local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = 0;
  local_1c = 0;
  local_11 = *(char *)((int)param_4 + 9);
  if (local_11 == '\0') {
    local_11 = param_5;
  }
  if (param_3 < *param_4) {
    local_20 = local_11;
    if (local_11 == '\x01') {
      local_1c = *param_4 - param_3;
    }
    else if (local_11 == '\x02') {
      local_18 = *param_4 - param_3;
    }
    else if (local_11 == '\x03') {
      local_18 = (*param_4 - param_3) / 2;
      local_1c = (*param_4 - param_3) - local_18;
    }
  }
  FUN_0040c080(local_10,8);
  FUN_0040de00(local_10,(int)param_4 + 0xf,(uint)*(byte *)((int)param_4 + 0xe));
  uVar2 = CONCAT44(local_28._4_4_,(undefined4)local_28);
  for (; local_28 = uVar2, 0 < local_18; local_18 = local_18 + -1) {
    move<>(&param_2);
    uVar2 = FUN_004e6cb0(local_10);
    param_2 = (undefined4)((ulonglong)uVar2 >> 0x20);
  }
  puVar1 = (undefined4 *)move<>(&param_2);
  puVar1 = FUN_004f6610(param_6,&local_34,*puVar1);
  uVar2 = CONCAT44(local_30._4_4_,(undefined4)local_30);
  param_2 = *puVar1;
  for (; local_30 = uVar2, 0 < local_1c; local_1c = local_1c + -1) {
    move<>(&param_2);
    uVar2 = FUN_004e6cb0(local_10);
    param_2 = (undefined4)((ulonglong)uVar2 >> 0x20);
  }
  *param_1 = param_2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

