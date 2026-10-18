/* Entry: 0x0049cf80; symbol: FUN_0049cf80; body bytes: 878 */

int * __thiscall FUN_0049cf80(void *this,int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  void *local_38;
  int local_34;
  undefined4 local_30;
  int local_2c;
  undefined4 local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  void *local_14;
  int local_10;
  int local_c;
  int *local_8;
  
  *(undefined4 *)this = 0;
  *(undefined4 *)((int)this + 4) = 0;
  local_8 = (int *)this;
  FUN_00425cc0((undefined4 *)((int)this + 8));
  local_8[3] = 0;
  local_8[4] = 0;
  local_8[5] = 0;
  local_8[6] = 0;
  local_8[7] = 0;
  local_8[8] = 0;
  if (DAT_005c4edc == 0) {
    _memset(local_8,0,0x24);
  }
  else {
    FUN_0049e050(local_8,param_4);
    *local_8 = param_1;
    local_8[1] = param_2;
    iVar1 = FUN_0041f610(param_1 * 4 - 1);
    local_8[3] = iVar1;
    iVar1 = FUN_0041f610(param_1 * 4 - 1);
    local_8[4] = iVar1;
    iVar1 = FUN_0041f610(param_1 * 0x1c * param_2);
    local_8[5] = iVar1;
    iVar1 = FUN_0041f610(param_1 * 0xc * param_2);
    local_8[6] = iVar1;
    if (param_3 == 0) {
      piVar2 = FUN_004de430(&local_3c,2,0);
      local_8[2] = *piVar2;
      local_18 = FUN_0044ced0(local_8 + 2);
      *(int **)(local_18 + 0x5c8) = local_8;
      FUN_00450160(local_18);
      for (local_10 = 0; local_10 < *local_8 + -1; local_10 = local_10 + 1) {
        puVar3 = FUN_004de430(&local_40,param_2,0);
        local_28 = *puVar3;
        *(undefined4 *)(local_8[3] + local_10 * 4) = local_28;
        local_2c = FUN_0044ced0((undefined4 *)(local_8[3] + local_10 * 4));
        *(int *)(local_8[4] + local_10 * 4) = local_2c;
        *(undefined1 *)(*(int *)(local_8[4] + local_10 * 4) + 0x499) = 0;
        *(byte *)(*(int *)(local_8[4] + local_10 * 4) + 0x4a3) =
             *(byte *)(*(int *)(local_8[4] + local_10 * 4) + 0x4a3) & 0xfc;
      }
    }
    else {
      if (DAT_005b87f8 == 0x280) {
        local_20 = 0xd;
      }
      else {
        if (DAT_005b87f8 == 0x3c0) {
          local_1c = 0xe;
        }
        else {
          local_1c = 0xf;
        }
        local_20 = local_1c;
      }
      local_24 = local_20;
      piVar2 = FUN_004de360(&local_44,2,local_20);
      local_8[2] = *piVar2;
      local_14 = (void *)FUN_0044ced0(local_8 + 2);
      *(int **)((int)local_14 + 0x5c8) = local_8;
      FUN_00450160((int)local_14);
      FUN_00450350(local_14,0x1b);
      for (local_c = 0; local_c < *local_8 + -1; local_c = local_c + 1) {
        puVar3 = FUN_004de360(&local_48,param_2,local_24);
        local_30 = *puVar3;
        *(undefined4 *)(local_8[3] + local_c * 4) = local_30;
        local_34 = FUN_0044ced0((undefined4 *)(local_8[3] + local_c * 4));
        *(int *)(local_8[4] + local_c * 4) = local_34;
        *(undefined1 *)(*(int *)(local_8[4] + local_c * 4) + 0x499) = 0;
        *(byte *)(*(int *)(local_8[4] + local_c * 4) + 0x4a3) =
             *(byte *)(*(int *)(local_8[4] + local_c * 4) + 0x4a3) & 0xfc;
        local_38 = *(void **)(local_8[4] + local_c * 4);
        FUN_00450350(local_38,0x1b);
      }
    }
  }
  return local_8;
}

