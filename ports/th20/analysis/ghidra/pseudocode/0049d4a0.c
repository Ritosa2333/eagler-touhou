/* Entry: 0x0049d4a0; symbol: FUN_0049d4a0; body bytes: 271 */

undefined4 __fastcall FUN_0049d4a0(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  void *this;
  undefined4 uVar3;
  undefined4 local_24;
  void *local_20;
  void *local_1c;
  void *local_18;
  int local_14;
  int local_10;
  int local_c;
  int *local_8;
  
  local_10 = param_1;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 0x1303c));
  for (local_c = 0; local_c < 0x400; local_c = local_c + 1) {
    local_18 = (void *)(local_10 + 0x3c);
    puVar1 = (undefined4 *)FUN_00414580(local_18,local_c);
    iVar2 = FUN_0044ced0(puVar1);
    if (iVar2 == 0) {
      local_1c = (void *)(local_10 + 0x3c);
      uVar3 = 0;
      this = (void *)FUN_00414580(local_1c,local_c);
      FUN_004117a0(this,uVar3);
    }
  }
  local_20 = (void *)(local_10 + 0x103c);
  local_8 = (int *)FUN_00449490(local_20,0);
  for (local_14 = 0; local_14 < 0x400; local_14 = local_14 + 1) {
    if (-1 < *local_8) {
      if (local_8[3] < 1) {
        FUN_0049db70(&local_24,*local_8,local_8 + 4,local_8[2]);
        *local_8 = -1;
      }
      else {
        local_8[3] = local_8[3] + -1;
      }
    }
    local_8 = local_8 + 0x12;
  }
  return 1;
}

