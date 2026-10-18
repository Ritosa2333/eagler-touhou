/* Entry: 0x004ee7b0; symbol: FUN_004ee7b0; body bytes: 131 */

void __thiscall FUN_004ee7b0(void *this,int param_1,byte param_2,undefined4 param_3)

{
  uint *puVar1;
  uint uVar2;
  undefined1 local_14 [4];
  uint *local_10;
  uint local_c;
  uint *local_8;
  
  local_c = *(uint *)((int)this + param_1 * 4) & 0xfffffff;
  uVar2 = 4;
  local_8 = (uint *)this;
  puVar1 = (uint *)move<>(param_3);
  FUN_00543e20((uint *)((int)local_8 + local_c + 4),puVar1,uVar2);
  local_10 = local_8 + param_1;
  FUN_004fcdf0(local_10,param_2);
  if (param_1 == -1) {
    puVar1 = FUN_004f4e70(local_14,local_c + 4);
    *local_8 = *puVar1;
  }
  return;
}

