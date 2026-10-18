/* Entry: 0x0049c680; symbol: FUN_0049c680; body bytes: 104 */

void __thiscall
FUN_0049c680(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004398a0(&local_10);
  local_10 = param_3;
  local_c = param_4;
  puVar2 = &local_10;
  puVar1 = (undefined4 *)FUN_0044cca0((int)this);
  FUN_00439640((void *)((int)this + 0x1e0),param_1,param_2,puVar1,puVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

