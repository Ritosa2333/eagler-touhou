/* Entry: 0x0051c6f0; symbol: FUN_0051c6f0; body bytes: 175 */

void __thiscall
FUN_0051c6f0(void *this,undefined4 param_1,undefined4 param_2,uint param_3,uint param_4)

{
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004291e0(local_20,param_3 >> 0x10 & 0xff,param_3 >> 8 & 0xff,param_3 & 0xff);
  FUN_004291e0(local_14,param_4 >> 0x10 & 0xff,param_4 >> 8 & 0xff,param_4 & 0xff);
  FUN_004395d0((void *)((int)this + 0xe0),param_1,param_2,local_20,local_14);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

