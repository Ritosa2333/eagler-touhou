/* Entry: 0x004a8760; symbol: FUN_004a8760; body bytes: 443 */

undefined4 __fastcall FUN_004a8760(int param_1)

{
  float fVar1;
  undefined4 *puVar2;
  void *pvVar3;
  undefined4 uVar4;
  float10 fVar5;
  undefined4 *local_14;
  undefined4 *local_10;
  float local_c;
  
  if (0.0 < *(float *)(param_1 + 0xc4)) {
    fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar1 = (float)fVar5;
    local_c = fVar1 - fVar1 * *(float *)(param_1 + 0xc4);
    if (local_c <= DAT_0056c8cc) {
      if (local_c < 0.0) {
        local_c = 0.0;
      }
    }
    else {
      local_c = DAT_0056c8cc;
    }
    FUN_004292a0(&DAT_005aefe4,local_c);
    local_14 = (undefined4 *)FUN_00414f30(param_1 + 0x94);
    puVar2 = (undefined4 *)FUN_00414f70(param_1 + 0x94);
    for (; local_14 != puVar2; local_14 = local_14 + 5) {
      pvVar3 = (void *)FUN_0044ced0(local_14);
      if (pvVar3 != (void *)0x0) {
        FUN_0045d130(pvVar3,*(undefined4 *)(param_1 + 0xc4));
      }
    }
    uVar4 = FUN_004a8260(param_1 + 0x88);
    FUN_004292a0(&DAT_005aefe4,fVar1);
    *(uint *)(param_1 + 0x354) = *(uint *)(param_1 + 0x354) | 0x10000;
  }
  else {
    if ((*(uint *)(param_1 + 0x354) >> 0x10 & 1) != 0) {
      local_10 = (undefined4 *)FUN_00414f30(param_1 + 0x94);
      puVar2 = (undefined4 *)FUN_00414f70(param_1 + 0x94);
      for (; local_10 != puVar2; local_10 = local_10 + 5) {
        pvVar3 = (void *)FUN_0044ced0(local_10);
        if (pvVar3 != (void *)0x0) {
          FUN_0045d130(pvVar3,0);
        }
      }
    }
    uVar4 = FUN_004a8260(param_1 + 0x88);
  }
  return uVar4;
}

