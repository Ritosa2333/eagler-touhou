/* Entry: 0x004eac00; symbol: FUN_004eac00; body bytes: 231 */

void __cdecl FUN_004eac00(undefined4 *param_1,undefined4 param_2,double param_3)

{
  bool bVar1;
  uint *puVar2;
  undefined4 *puVar3;
  undefined8 uVar4;
  double dVar5;
  uint *local_2c;
  uint *local_24;
  undefined1 local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = (uint *)local_20;
  bVar1 = FUN_004f3790();
  if (bVar1) {
    bVar1 = FUN_004fffa0(param_3);
    if (bVar1) {
      local_20[0] = 0x2d;
      local_24 = (uint *)(local_20 + 1);
    }
    FUN_00543e20(local_24,(uint *)&DAT_0056f58c,3);
    local_24 = (uint *)((int)local_24 + 3);
  }
  if (local_24 == (uint *)local_20) {
    dVar5 = param_3;
    puVar2 = (uint *)FUN_004f3310((int)local_20);
    uVar4 = FUN_00500130((uint *)local_20,puVar2,dVar5);
    local_2c = (uint *)uVar4;
    local_24 = local_2c;
  }
  puVar3 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,(undefined1 *)local_24,*puVar3);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

