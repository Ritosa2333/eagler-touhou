/* Entry: 0x00520c80; symbol: FUN_00520c80; body bytes: 806 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00520c80(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int local_2c;
  int local_24;
  int local_20;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(int *)(param_1 + 0x20) == 1) || (*(int *)(param_1 + 0x20) == 2)) {
    FUN_00422dd0(&local_14,DAT_0056fa28,DAT_0056fe7c,0);
    FUN_004c6160(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004c6180(DAT_005c0698,5);
    local_24 = 0;
    while( true ) {
      bVar1 = FUN_00461070((void *)(param_1 + 0x154),10);
      if (bVar1) {
        local_2c = 10;
      }
      else {
        local_2c = FUN_0040ff90(param_1 + 0x154);
      }
      if ((local_2c <= local_24) ||
         (iVar4 = *(int *)(param_1 + 0x56d0) + local_24, *(int *)(param_1 + 0x47c) <= iVar4)) break;
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x24),iVar4);
      if (CONCAT31(extraout_var,bVar1) == 0) {
        FUN_00470ad0(DAT_005c0698,0xffb0b0b0);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffff00);
      }
      iVar2 = FUN_004640e0(DAT_005c6108);
      pcVar3 = (char *)FUN_00411c10((void *)(iVar2 + 0x36),iVar4);
      if (*pcVar3 == '\0') {
        FUN_0040ddd0(&stack0xffffffb4,&DAT_00575508);
        FUN_0046c210(DAT_005c0698,&local_14);
      }
      else {
        FUN_0040ddd0(&stack0xffffffb8,(char *)(param_1 + 0xc90 + iVar4 * 0x42));
        FUN_0046c210(DAT_005c0698,&local_14);
      }
      local_10 = local_10 + _DAT_0056ec8c;
      local_24 = local_24 + 1;
    }
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    local_14 = DAT_0056f090;
    local_10 = DAT_0056f10c;
    FUN_004c6180(DAT_005c0698,4);
    for (local_20 = 0; local_20 < *(int *)(param_1 + 0x480); local_20 = local_20 + 1) {
      iVar4 = FUN_004640e0(DAT_005c6108);
      pcVar3 = (char *)FUN_00411c10((void *)(iVar4 + 0x36),*(int *)(param_1 + 0x484));
      if ((*pcVar3 == '\0') && (*(int *)(param_1 + 0x48c) != 0)) {
        FUN_0040ddd0(&stack0xffffffb8,(&PTR_DAT_005b0a88)[local_20]);
        FUN_0046c210(DAT_005c0698,&local_14);
      }
      else {
        FUN_0040ddd0(&stack0xffffffb8,
                     (char *)(param_1 + 0x14d0 + *(int *)(param_1 + 0x484) * 0x210 + local_20 * 0x42
                             ));
        FUN_0046c210(DAT_005c0698,&local_14);
      }
      local_10 = local_10 + _DAT_00572644;
    }
    FUN_004c6160(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004c6180(DAT_005c0698,2);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

