/* Entry: 0x004aac00; symbol: FUN_004aac00; body bytes: 386 */

void __thiscall FUN_004aac00(void *this,undefined4 *param_1,float *param_2,float param_3)

{
  bool bVar1;
  float *pfVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  float10 fVar4;
  undefined4 local_3c;
  undefined4 *local_38;
  int *local_34;
  void *local_30;
  float local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056867d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_24 = (void *)0x0;
  param_3 = param_3 * param_3;
  local_28 = (void *)((int)this + 0x108);
  local_30 = this;
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_28,local_1c);
  local_8 = 0;
  local_34 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,local_34);
    if (!bVar1) break;
    local_38 = (undefined4 *)FUN_0040c300(local_1c);
    local_20 = (void *)FUN_0040c300(local_38);
    if (((((*(uint *)((int)local_20 + 0x350) & 1) == 0) &&
         ((*(uint *)((int)local_20 + 0x350) >> 5 & 1) == 0)) &&
        ((*(uint *)((int)local_20 + 0x354) >> 0xb & 1) == 0)) &&
       ((*(uint *)((int)local_20 + 0x354) >> 10 & 1) == 0)) {
      pfVar2 = (float *)FUN_0047a2c0((int)local_20);
      fVar4 = FUN_004560d0(param_2,pfVar2);
      local_2c = (float)fVar4;
      if (local_2c < param_3) {
        local_24 = local_20;
        param_3 = local_2c;
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_00425cc0(param_1);
  if (local_24 == (void *)0x0) {
    FUN_004117a0(param_1,0);
  }
  else {
    puVar3 = FUN_00498f90(local_24,&local_3c);
    *param_1 = *puVar3;
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

