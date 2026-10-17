/* Entry: 0x004ea180; symbol: FUN_004ea180; body bytes: 225 */

void __cdecl FUN_004ea180(undefined4 *param_1,undefined4 param_2,float param_3)

{
  bool bVar1;
  uint *puVar2;
  undefined4 *puVar3;
  undefined8 uVar4;
  float fVar5;
  uint *local_2c;
  uint *local_24;
  undefined1 local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = (uint *)local_20;
  bVar1 = FUN_004f3750();
  if (bVar1) {
    bVar1 = FUN_004fff60((uint)param_3);
    if (bVar1) {
      local_20[0] = 0x2d;
      local_24 = (uint *)(local_20 + 1);
    }
    FUN_00543e20(local_24,(uint *)&DAT_0056f58c,3);
    local_24 = (uint *)((int)local_24 + 3);
  }
  if (local_24 == (uint *)local_20) {
    fVar5 = param_3;
    puVar2 = (uint *)FUN_004f3310((int)local_20);
    uVar4 = FUN_005000b0((uint *)local_20,puVar2,fVar5);
    local_2c = (uint *)uVar4;
    local_24 = local_2c;
  }
  puVar3 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,(undefined1 *)local_24,*puVar3);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

