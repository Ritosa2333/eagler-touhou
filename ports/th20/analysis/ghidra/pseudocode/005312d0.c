/* Entry: 0x005312d0; symbol: FUN_005312d0; body bytes: 295 */

bool __fastcall FUN_005312d0(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined1 local_2c [12];
  void *local_20;
  void *local_1c;
  int local_18;
  void *local_14;
  float *local_10;
  void *local_c;
  int local_8;
  
  local_c = (void *)(param_1 + 0x14);
  local_8 = param_1;
  bVar1 = FUN_00423590(local_c,0);
  if (!bVar1) {
    FUN_0044f130();
    FUN_0044fa30();
    local_1c = (void *)FUN_00464080(0);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0051b920(iVar2);
    iVar3 = FUN_0040ff90(local_8 + 0x14);
    local_18 = FUN_00534260();
    local_18 = (iVar2 * iVar3) / local_18;
    FUN_004be940(local_1c,local_18);
    local_20 = (void *)(local_8 + 0x14);
    FUN_00429420(local_20);
  }
  else {
    local_14 = (void *)FUN_00498fd0(0);
    local_10 = (float *)FUN_00422dd0(local_2c,0,DAT_0056fa28,0);
    FUN_004c3c90(local_14,6,local_10,0xffffffff,DAT_0056e0f8 / DAT_0056c8d0,DAT_0056fd34,0,0,
                 0xffffffff);
    FUN_0044fcd0((undefined4 *)(local_8 + 0x38));
  }
  return bVar1;
}

