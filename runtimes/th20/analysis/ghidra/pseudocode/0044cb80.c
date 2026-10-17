/* Entry: 0x0044cb80; symbol: FUN_0044cb80; body bytes: 236 */

void __thiscall FUN_0044cb80(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  float10 fVar2;
  undefined1 local_3c [12];
  undefined1 local_30 [12];
  void *local_24;
  void *local_20;
  void *local_1c;
  void *local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (void *)((int)this + 0x5bc);
  local_18 = this;
  local_20 = FUN_00429570(local_1c,local_30,(float *)((int)this + 0x2c));
  puVar1 = (undefined4 *)FUN_00429570(local_20,local_3c,(float *)((int)local_18 + 0x484));
  *param_1 = *puVar1;
  param_1[1] = puVar1[1];
  param_1[2] = puVar1[2];
  if ((*(int *)((int)local_18 + 0x558) != 0) && ((*(uint *)((int)local_18 + 0x49c) >> 0xc & 1) == 0)
     ) {
    if ((*(uint *)((int)local_18 + 0x49c) >> 5 & 1) != 0) {
      fVar2 = FUN_00437930(*(int *)((int)local_18 + 0x558));
      FUN_00458fa0(param_1,(float *)param_1,(float *)param_1,(float)fVar2);
    }
    FUN_00422e10(local_14);
    local_24 = *(void **)((int)local_18 + 0x558);
    FUN_0044cb80(local_24,local_14);
    FUN_004296e0(param_1,local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

