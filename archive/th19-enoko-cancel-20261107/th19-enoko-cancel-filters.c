// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4ee0d0

/* WARNING: Removing unreachable block (ram,0x004ee19d) */
/* WARNING: Restarted to delay deadcode elimination for space: stack */

undefined4
FUN_004ee0d0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5)

{
  char cVar1;
  undefined4 *puVar2;
  int *piVar3;
  undefined1 local_a8 [12];
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  int local_84;
  undefined4 local_80;
  undefined1 local_78 [4];
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  int local_68 [3];
  undefined4 local_5c;
  int *local_58;
  int *local_54;
  int local_50;
  undefined4 local_48;
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  uint local_34;
  int *local_30;
  undefined4 local_2c;
  int local_28;
  int local_24;
  int local_20;
  undefined1 local_1c;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  int local_14;
  
  FUN_00402780();
  FUN_00402780();
  puVar2 = (undefined4 *)FUN_00407350(local_90);
  local_40 = *puVar2;
  local_3c = puVar2[1];
  local_38 = puVar2[2];
  puVar2 = (undefined4 *)FUN_00403b00(local_9c,&local_40);
  local_74 = *puVar2;
  local_70 = puVar2[1];
  local_6c = puVar2[2];
  puVar2 = (undefined4 *)FUN_00404010(local_a8,&local_40);
  local_40 = *puVar2;
  local_3c = puVar2[1];
  local_38 = puVar2[2];
  piVar3 = (int *)FUN_0040d0a0(local_78,local_20 + 0x60);
  local_84 = *piVar3;
  local_48 = 0x7d1;
  local_2c = 2000;
  local_68[1] = 2000;
  local_54 = local_68;
  local_80 = 2000;
  local_15 = DAT_005a83a8;
  local_17 = DAT_005a83a8;
  local_58 = &local_84;
  local_24 = 2000;
  local_68[0] = local_84;
  local_68[2] = local_84;
  local_5c = local_80;
  local_50 = local_84;
  local_44 = local_84;
  local_28 = local_84;
  while( true ) {
    local_1c = local_18;
    local_16 = local_24 == 0;
    local_34 = (uint)(byte)local_16;
    if ((bool)local_16) break;
    local_14 = thunk_FUN_0040d1a0();
    if ((((*(int *)(local_14 + 0x1044) == 2) || (*(int *)(local_14 + 0x1044) == 1)) &&
        (*(int *)(local_14 + 0x14) == 0)) &&
       ((cVar1 = FUN_004d44e0(param_3,*(undefined4 *)(local_14 + 0x650),
                              *(undefined4 *)(local_14 + 0x654),
                              *(float *)(local_14 + 0x674) * *(float *)(local_14 + 0xfd4)),
        cVar1 != '\0' &&
        (cVar1 = FUN_004d44e0(0,*(undefined4 *)(local_14 + 0x650),*(undefined4 *)(local_14 + 0x654),
                              *(float *)(local_14 + 0x674) * *(float *)(local_14 + 0xfd4)),
        cVar1 != '\0')))) {
      *(int *)(local_20 + 0x815708) = *(int *)(local_20 + 0x815708) + 1;
      *(undefined4 *)(local_14 + 0x698) = param_5;
      FUN_004eeb30(param_4);
    }
    local_30 = &local_28;
    local_28 = local_28 + 0x1088;
    local_24 = local_24 + -1;
  }
  return 0;
}


// Reference 00419229
// Reference 004192c0
// Reference 0041c4ae
// Reference 0041db1a
// Reference 0053dbcb
// Reference 0054ac6b
// Reference 00503e79
// Address 5303f0

int __thiscall FUN_005303f0(int param_1,undefined8 *param_2,undefined8 *param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 in_XMM3_Da;
  
  iVar1 = *(int *)(param_1 + 0x34);
  if (*(int *)(param_1 + 0x24 + iVar1 * 4) != 0) {
    iVar1 = iVar1 + 1;
    *(int *)(param_1 + 0x34) = iVar1;
    if (3 < iVar1) {
      *(undefined4 *)(param_1 + 0x34) = 3;
      iVar1 = 3;
    }
  }
  piVar3 = *(int **)(param_1 + 0x14);
  if (piVar3 == (int *)0x0) {
    iVar2 = 0;
  }
  else {
    iVar2 = piVar3[1];
  }
  *(int *)(param_1 + 0x24 + iVar1 * 4) = iVar2;
  iVar1 = 0;
  *(undefined8 *)(param_1 + 0x40) = *param_2;
  *(undefined4 *)(param_1 + 0x48) = *(undefined4 *)(param_2 + 1);
  *(undefined8 *)(param_1 + 0x4c) = *param_3;
  *(undefined4 *)(param_1 + 0x54) = *(undefined4 *)(param_3 + 1);
  if (piVar3 != (int *)0x0) {
    while( true ) {
      piVar3 = (int *)*piVar3;
      if ((piVar3[7] != 1) && ((*(byte *)(piVar3 + 6) & 1) != 0)) {
        iVar2 = (**(code **)(*piVar3 + 0x2c))(param_2,param_3,in_XMM3_Da,0,1);
        iVar1 = iVar1 + iVar2;
      }
      piVar3 = *(int **)(param_1 + 0x24 + *(int *)(param_1 + 0x34) * 4);
      if (piVar3 == (int *)0x0) break;
      *(int *)(param_1 + 0x24 + *(int *)(param_1 + 0x34) * 4) = piVar3[1];
    }
  }
  *(undefined4 *)(param_1 + 0x24 + *(int *)(param_1 + 0x34) * 4) = 0;
  if (0 < *(int *)(param_1 + 0x34)) {
    *(int *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + -1;
  }
  return iVar1;
}


// Reference 00419242
// Reference 004192e6
// Reference 0041c4c5
// Reference 0041db31
// Reference 0053dbe7
// Reference 0054ac87
