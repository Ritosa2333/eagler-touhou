/* Entry: 0x004f31e0; symbol: FUN_004f31e0; body bytes: 208 */

void __cdecl
FUN_004f31e0(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined1 *local_2c;
  undefined1 *local_28;
  undefined1 *local_24;
  undefined1 *local_20;
  undefined4 *local_1c;
  int local_18;
  undefined4 local_14;
  undefined1 *local_10;
  undefined1 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&param_2,(char **)&param_3);
  local_10 = (undefined1 *)FUN_0040b2a0(&param_2);
  local_c = (undefined1 *)FUN_0040b2a0(&param_3);
  local_18 = FUN_004eca30((int *)&local_10,(int *)&local_c);
  local_1c = (undefined4 *)move<>(&param_4);
  local_14 = FUN_004ec900(local_1c);
  puVar1 = (undefined4 *)move<>(&local_14);
  local_20 = (undefined1 *)*puVar1;
  local_24 = local_c;
  local_28 = local_10;
  local_2c = FUN_004e7be0(local_10,local_c,local_20);
  FUN_004ee740(&param_4,&local_2c);
  *param_1 = param_4;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

