/* Entry: 0x004f5b10; symbol: FUN_004f5b10; body bytes: 128 */

undefined4 * __thiscall FUN_004f5b10(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined1 local_30 [8];
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  int local_18;
  void *local_14;
  
  local_14 = this;
  local_1c = FUN_0040c300((undefined4 *)this);
  local_18 = *(int *)((int)local_14 + 8);
  puVar1 = FUN_004e6c00(local_30,local_1c,local_18);
  local_28 = *puVar1;
  local_24 = puVar1[1];
  puVar1 = (undefined4 *)move<>(&stack0x00000008);
  local_20 = *puVar1;
  FUN_004e9af0(param_1);
  return param_1;
}

