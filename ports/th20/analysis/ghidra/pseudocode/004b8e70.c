/* Entry: 0x004b8e70; symbol: FUN_004b8e70; body bytes: 233 */

void __thiscall
FUN_004b8e70(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 local_2c;
  void *local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  void *local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = this;
  FUN_004b7db0((int)this);
  FUN_00422dd0(local_14,param_1,param_2,0);
  local_24 = FUN_004b8250(DAT_005c06a4);
  puVar1 = FUN_004790e0(&local_2c,(uint *)"front",param_4 + 0x10e,local_14,0,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_18 + 0x6c) = *puVar1;
  local_1c = 0;
  do {
    local_28 = (void *)((int)local_18 + 0x6c);
    local_20 = FUN_0044c670(local_28,-1,local_1c);
    if (local_20 != 0) {
      iVar2 = FUN_0045d0c0(local_20);
      *(undefined4 *)(iVar2 + 0x10) = param_3;
    }
    local_1c = local_1c + 1;
  } while (local_20 != 0);
  *(int *)((int)local_18 + 0x13c) = param_4;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

