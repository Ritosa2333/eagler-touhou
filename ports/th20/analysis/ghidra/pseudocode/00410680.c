/* Entry: 0x00410680; symbol: FUN_00410680; body bytes: 83 */

bool __cdecl FUN_00410680(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  bool bVar2;
  undefined4 local_14 [3];
  int local_8;
  
  puVar1 = (undefined4 *)FUN_00411720(local_14,param_1,param_2);
  local_8 = FUN_0040c300(puVar1);
  if (local_8 == 0) {
    bVar2 = false;
  }
  else {
    FUN_0040ffb0(param_2);
    bVar2 = local_8 != 1;
  }
  return bVar2;
}

