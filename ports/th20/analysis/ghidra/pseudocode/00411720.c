/* Entry: 0x00411720; symbol: FUN_00411720; body bytes: 111 */

void __cdecl FUN_00411720(undefined4 *param_1,undefined4 *param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 local_28 [3];
  undefined1 local_1c [8];
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar2 = (undefined4 *)FUN_0040f100(local_28,param_2,3);
  local_14 = *puVar2;
  local_10 = puVar2[1];
  local_c = puVar2[2];
  puVar2 = (undefined4 *)make_error_code(local_1c,local_c);
  uVar1 = puVar2[1];
  *param_3 = *puVar2;
  param_3[1] = uVar1;
  *param_1 = local_14;
  param_1[1] = local_10;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

