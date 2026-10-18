/* Entry: 0x004e7640; symbol: FUN_004e7640; body bytes: 118 */

undefined4 * __thiscall FUN_004e7640(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  void *local_14;
  undefined4 local_10;
  void *local_c;
  undefined4 *local_8;
  
                    /* WARNING: Load size is inaccurate */
  local_c = *this;
  local_8 = (undefined4 *)this;
  puVar1 = FUN_004fc010(local_c,&local_28);
  local_1c = *puVar1;
  local_20 = local_8[1];
  local_14 = (void *)*local_8;
  local_10 = local_1c;
  puVar1 = FUN_004ff9e0(local_14,&local_2c);
  local_24 = *puVar1;
  local_18 = local_24;
  FUN_004e9ae0(param_1,local_24);
  return param_1;
}

