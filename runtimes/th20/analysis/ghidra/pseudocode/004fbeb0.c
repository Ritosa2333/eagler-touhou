/* Entry: 0x004fbeb0; symbol: FUN_004fbeb0; body bytes: 112 */

void __fastcall FUN_004fbeb0(int param_1)

{
  undefined4 *puVar1;
  undefined4 local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  int local_8;
  
  local_8 = param_1;
  local_c = FUN_00412730(param_1);
  FUN_004fb980(local_8);
  local_10 = local_8 + 0x10;
  local_14 = FUN_00470540(local_c);
  local_14 = local_8 + 0x14 + local_14;
  move<>(local_8 + 0x10);
  puVar1 = FUN_004e96a0(&local_18,local_8 + 0x14,local_14);
  *(undefined4 *)(local_8 + 0x10) = *puVar1;
  return;
}

