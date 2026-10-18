/* Entry: 0x0044b9f0; symbol: FUN_0044b9f0; body bytes: 243 */

int * __thiscall FUN_0044b9f0(void *this,int *param_1)

{
  uint uVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  int local_3c [2];
  int local_34 [2];
  int *local_2c;
  int *local_28;
  int *local_24;
  int *local_20;
  int *local_1c;
  void *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005685ce;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = 0;
  local_18 = this;
  iVar2 = FUN_0040c300((undefined4 *)this);
  if (iVar2 == 0) {
    local_24 = FUN_004119b0(local_3c,0,0);
    local_8 = 0;
    local_14 = local_14 | 1;
    local_20 = local_24;
    local_1c = local_24;
  }
  else {
    iVar2 = FUN_0040ff90((int)local_18);
    local_28 = FUN_004119b0(local_34,(int)local_18,iVar2);
    local_14 = local_14 | 2;
    local_1c = local_28;
  }
  local_2c = local_1c;
  iVar2 = local_1c[1];
  *param_1 = *local_1c;
  param_1[1] = iVar2;
  uVar1 = local_14 | 4;
  if ((local_14 & 2) != 0) {
    local_14 = local_14 & 0xfffffffd | 4;
    FUN_00411b00(local_34);
    uVar1 = local_14;
  }
  local_14 = uVar1;
  local_8 = 0xffffffff;
  if ((local_14 & 1) != 0) {
    local_14 = local_14 & 0xfffffffe;
    FUN_00411b00(local_3c);
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

