/* Entry: 0x00534ab0; symbol: FUN_00534ab0; body bytes: 581 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00534ab0(void)

{
  int iVar1;
  int *piVar2;
  float10 fVar3;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  void *local_6c;
  undefined4 *local_68;
  undefined4 *local_64;
  int local_60;
  int local_5c;
  int local_58;
  undefined1 local_51;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  float local_38;
  float local_34;
  undefined1 local_30 [8];
  undefined1 local_28;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_0041cab0(iVar1);
  if (iVar1 == 7) {
    local_64 = (undefined4 *)(**(code **)(**(int **)(local_58 + 0x28) + 0x60))();
  }
  else {
    iVar1 = FUN_00460830(0);
    local_64 = (undefined4 *)FUN_00460850(iVar1);
  }
  local_68 = local_64;
  local_18 = *local_64;
  local_14 = local_64[1];
  local_10 = local_64[2];
  FUN_004a2800(*(void **)(local_58 + 0x40));
  *(undefined4 *)(local_58 + 0x40) = 0;
  piVar2 = FUN_004712d0();
  *(int **)(local_58 + 0x40) = piVar2;
  FUN_00423520((void *)(local_58 + 0x44),0);
  local_6c = *(void **)(local_58 + 0x40);
  FUN_0052cd50(local_6c,FUN_00532720);
  FUN_0047ba30(&local_50);
  FUN_00429210(&local_c,0.0);
  local_50 = local_18;
  local_4c = local_14;
  local_48 = local_10;
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_0041cab0(iVar1);
  local_51 = iVar1 != 7;
  local_34 = DAT_0056e0f0 / _DAT_0056ec8c;
  local_28 = local_51;
  FUN_0045d650(local_30,0xffffc080);
  for (local_5c = 0; local_5c < 0x18; local_5c = local_5c + 1) {
    fVar3 = FUN_004292e0(&local_c);
    local_38 = (float)fVar3;
    local_70 = FUN_00437520(0);
    FUN_0049db70(&local_78,6,&local_50,0);
    FUN_004530c0(&local_c,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056f7b4);
  }
  local_34 = DAT_0056e0f8 / _DAT_0056ec8c;
  for (local_60 = 0; local_60 < 0x18; local_60 = local_60 + 1) {
    fVar3 = FUN_004292e0(&local_c);
    local_38 = (float)fVar3;
    local_74 = FUN_00437520(0);
    FUN_0049db70(&local_7c,6,&local_50,0);
    FUN_00472060(&local_c,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056f7b4);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

