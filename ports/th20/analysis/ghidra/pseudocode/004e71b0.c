/* Entry: 0x004e71b0; symbol: FUN_004e71b0; body bytes: 124 */

undefined4 * __thiscall FUN_004e71b0(void *this,undefined4 *param_1,float param_2)

{
  undefined4 *puVar1;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  int *local_20;
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
  local_20 = (int *)local_8[1];
  local_14 = (void *)*local_8;
  local_10 = local_1c;
  puVar1 = FUN_004ff9e0(local_14,&local_2c);
  local_24 = *puVar1;
  local_18 = local_24;
  FUN_004ea270(param_1,local_24,param_2,local_20);
  return param_1;
}

