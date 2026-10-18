/* Entry: 0x004e6530; symbol: FUN_004e6530; body bytes: 203 */

undefined4 __thiscall
FUN_004e6530(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
            undefined4 param_9,undefined4 param_10)

{
  int local_8;
  
  local_8 = 0;
  while( true ) {
    if (3 < local_8) {
      return 0;
    }
    if (*(int *)((int)this + local_8 * 0x28 + 0x18) < 0) break;
    local_8 = local_8 + 1;
  }
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x18) = param_1;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x1c) = param_2;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x20) = param_3;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x24) = param_4;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x28) = param_5;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x2c) = param_6;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x30) = param_7;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x34) = param_8;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x38) = param_9;
  *(undefined4 *)((int)this + local_8 * 0x28 + 0x3c) = param_10;
  return 0;
}

