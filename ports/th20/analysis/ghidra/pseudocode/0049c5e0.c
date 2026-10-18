/* Entry: 0x0049c5e0; symbol: FUN_0049c5e0; body bytes: 155 */

void __thiscall FUN_0049c5e0(void *this,undefined4 param_1,undefined4 param_2,byte *param_3)

{
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004291e0(local_20,*(uint *)((int)this + 0x490) >> 0x10 & 0xff,
               *(uint *)((int)this + 0x490) >> 8 & 0xff,*(uint *)((int)this + 0x490) & 0xff);
  FUN_00429180(local_14,param_3);
  FUN_004395d0((void *)((int)this + 0xe0),param_1,param_2,local_20,local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

