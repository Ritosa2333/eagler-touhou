/* Entry: 0x00438d50; symbol: FUN_00438d50; body bytes: 96 */

void __thiscall
FUN_00438d50(void *this,undefined4 param_1,undefined4 param_2,byte *param_3,byte *param_4)

{
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00429180(local_20,param_3);
  FUN_00429180(local_14,param_4);
  FUN_004395d0((void *)((int)this + 0xe0),param_1,param_2,local_20,local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

