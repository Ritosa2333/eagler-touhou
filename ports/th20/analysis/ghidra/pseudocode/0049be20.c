/* Entry: 0x0049be20; symbol: FUN_0049be20; body bytes: 123 */

bool __fastcall FUN_0049be20(int param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  undefined1 local_2c [4];
  undefined1 local_28 [4];
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  void *local_18;
  undefined4 local_14;
  void *local_10;
  int local_c;
  
  local_10 = (void *)(param_1 + 0x54);
  local_c = param_1;
  puVar1 = (undefined4 *)FUN_004981a0(local_10,local_28);
  local_20 = *puVar1;
  local_18 = (void *)(local_c + 0x54);
  local_14 = local_20;
  puVar1 = (undefined4 *)begin(local_18,local_2c);
  local_24 = *puVar1;
  local_1c = local_24;
  uVar2 = FUN_0048a990();
  return (uVar2 & 0xff) != 0;
}

