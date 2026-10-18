/* Entry: 0x004e5240; symbol: FUN_004e5240; body bytes: 328 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004e5240(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int *piVar2;
  int iVar3;
  int local_18;
  undefined4 local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(&local_14,DAT_0056fb7c,DAT_0056fa28,0);
  for (local_18 = 0; local_18 < 0x19; local_18 = local_18 + 1) {
    bVar1 = FUN_0046b1b0((void *)(param_1 + 0x30),local_18);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_00470ad0(DAT_005c0698,0xff808080);
    }
    else {
      FUN_00470ad0(DAT_005c0698,0xffffff00);
    }
    piVar2 = (int *)FUN_00414580((void *)(param_1 + 0xf0),local_18);
    if (*piVar2 == 0) {
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"No.%.2d -------- --/--/-- ------ -- St-");
    }
    else {
      piVar2 = (int *)FUN_00414580((void *)(param_1 + 0xf0),local_18);
      iVar3 = FUN_00421810(*piVar2);
      FUN_004e5390(local_18,&local_14,iVar3);
    }
    local_10 = local_10 + _DAT_0056f2fc;
  }
  FUN_00488b00(DAT_005c0698,0);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

