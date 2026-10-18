/* Entry: 0x00438bd0; symbol: FUN_00438bd0; body bytes: 132 */

void __thiscall
FUN_00438bd0(void *this,undefined4 param_1,undefined4 param_2,byte param_3,byte param_4)

{
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = (uint)param_3;
  local_c = (uint)param_4;
  FUN_00439550((void *)((int)this + 0x2f4),param_1,param_2,&local_10,&local_c);
  if ((*(byte *)((int)this + 0x4a1) >> 2 & 7) == 0) {
    *(byte *)((int)this + 0x4a1) = *(byte *)((int)this + 0x4a1) & 0xe3 | 4;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

