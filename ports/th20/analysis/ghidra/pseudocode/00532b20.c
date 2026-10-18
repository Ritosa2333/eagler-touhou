/* Entry: 0x00532b20; symbol: FUN_00532b20; body bytes: 1013 */

undefined4 FUN_00532b20(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 *puVar4;
  undefined3 extraout_var_01;
  undefined4 local_4c;
  undefined4 *local_48;
  void *local_44;
  void *local_40;
  undefined4 *local_3c;
  undefined4 local_38;
  void *local_34;
  float *local_30;
  float local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  undefined4 local_18;
  float *local_14;
  void *local_10;
  int local_c;
  char local_6;
  char local_5;
  
  if (((DAT_005ba828 != 0) && (uVar2 = FUN_00534300(DAT_005ba828), (uVar2 & 0xff) != 0)) &&
     (*(int *)(local_c + 0x54) != 0)) {
    local_10 = (void *)(local_c + 0x20);
    FUN_0044ef90(local_10,1);
    FUN_004117a0((void *)(local_c + 0x20),0);
    *(undefined4 *)(local_c + 0x54) = 0;
    (**(code **)(**(int **)(local_c + 0x28) + 0x58))();
  }
  if (((DAT_005ba828 == 0) || (iVar3 = FUN_00464350(DAT_005ba828), iVar3 == 0)) ||
     ((DAT_005c06a4 == 0 || (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) != 0))
     )) {
    if (DAT_005c06a4 == 0) {
      return 1;
    }
    bVar1 = FUN_00478160(DAT_005c06a4);
    if (CONCAT31(extraout_var_01,bVar1) == 0) {
      return 1;
    }
    FUN_0044fcd0((undefined4 *)(local_c + 0x20));
    (**(code **)(**(int **)(local_c + 0x28) + 0x5c))();
    return 1;
  }
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_0041cab0(iVar3);
  if (iVar3 == 5) {
LAB_00532bfd:
    local_6 = '\0';
  }
  else {
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0041cab0(iVar3);
    if (iVar3 == 7) goto LAB_00532bfd;
    local_6 = '\x01';
  }
  local_5 = local_6;
  if (*(int *)(local_c + 0x54) == 2) {
    iVar3 = (**(code **)(**(int **)(local_c + 0x28) + 0x58))();
    if (iVar3 != 0) {
      *(undefined4 *)(local_c + 0x54) = 1;
      goto LAB_00532c61;
    }
    *(undefined4 *)(local_c + 0x74) = DAT_0056cd98;
    if (local_5 != '\0') {
      FUN_00423520((void *)(local_c + 100),0x3c);
    }
  }
  else {
LAB_00532c61:
    if (*(int *)(local_c + 0x54) != 1) goto LAB_00532da3;
    bVar1 = FUN_00485a10((undefined4 *)(local_c + 0x20));
    if (CONCAT31(extraout_var_00,bVar1) == 0) {
      local_18 = *(undefined4 *)(local_c + 0x24);
      iVar3 = FUN_00460830(0);
      local_14 = (float *)FUN_00460850(iVar3);
      puVar4 = FUN_004790e0(&local_4c,(uint *)&DAT_005762c4,0x1d,local_14,0,-1,(undefined4 *)0x0);
      *(undefined4 *)(local_c + 0x20) = *puVar4;
    }
    local_1c = (void *)(local_c + 0x20);
    iVar3 = FUN_00460830(0);
    puVar4 = (undefined4 *)FUN_00460850(iVar3);
    FUN_004645e0(local_1c,puVar4);
    iVar3 = (**(code **)(**(int **)(local_c + 0x28) + 0x54))();
    if (iVar3 == 0) goto LAB_00532da3;
    (**(code **)(**(int **)(local_c + 0x28) + 0x58))();
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0041cab0(iVar3);
    if (iVar3 == 6) {
      local_5 = '\0';
    }
    if (local_5 != '\0') {
      FUN_00423520((void *)(local_c + 100),4);
    }
    *(undefined4 *)(local_c + 0x74) = DAT_0056f7b4;
  }
  local_20 = (void *)(local_c + 0x20);
  FUN_0044ef90(local_20,1);
  FUN_004117a0((void *)(local_c + 0x20),0);
  *(undefined4 *)(local_c + 0x54) = 0;
  FUN_00534ab0();
  local_24 = (void *)FUN_00464080(0);
  FUN_004be940(local_24,0);
LAB_00532da3:
  local_28 = (void *)(local_c + 100);
  bVar1 = FUN_00461070(local_28,0);
  if ((bVar1) && (local_5 != '\0')) {
    local_34 = (void *)FUN_00478e80(0);
    local_2c = *(float *)(local_c + 0x74);
    iVar3 = FUN_00460830(0);
    local_30 = (float *)FUN_00460850(iVar3);
    FUN_0047cf60(local_34,local_30,local_2c,0,99999,0);
    local_40 = (void *)FUN_00478ee0(0);
    local_38 = *(undefined4 *)(local_c + 0x74);
    iVar3 = FUN_00460830(0);
    local_3c = (undefined4 *)FUN_00460850(iVar3);
    FUN_004caad0(local_40,local_3c,local_38,0,1);
    local_44 = (void *)(local_c + 100);
    FUN_00429420(local_44);
  }
  (**(code **)(**(int **)(local_c + 0x28) + 0x3c))();
  (**(code **)(**(int **)(local_c + 0x2c) + 0x40))();
  (**(code **)(**(int **)(local_c + 0x30) + 0x44))();
  (**(code **)(**(int **)(local_c + 0x34) + 0x48))();
  local_48 = (undefined4 *)(local_c + 0x10);
  FUN_00423540(local_48);
  return 1;
}

