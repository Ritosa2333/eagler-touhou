/* Entry: 0x00447250; symbol: FUN_00447250; body bytes: 104 */

void __thiscall FUN_00447250(void *this,void *param_1)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  uint *puVar3;
  undefined4 local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(local_10,8);
  FUN_004494f0(param_1,local_10);
  uVar1 = FUN_0040ff90((int)local_10);
  puVar2 = (undefined1 *)move<>(uVar1);
  puVar3 = (uint *)FUN_0040c300(local_10);
  FUN_0044b970(this,puVar3,puVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

