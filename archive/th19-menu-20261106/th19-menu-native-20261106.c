// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 5596a0

undefined4 __fastcall FUN_005596a0(int param_1)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined1 local_18 [4];
  undefined *local_14;
  void *local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00578971;
  local_10 = ExceptionList;
  ExceptionList = &local_10;
  local_14 = &DAT_0062b018;
  iVar2 = FUN_0046230c(&DAT_0062b018,DAT_005c3d80 ^ (uint)&stack0xfffffffc);
  if (iVar2 != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(5);
  }
  if (DAT_0062b044 == 0x7fffffff) {
    DAT_0062b044 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(6);
  }
  local_8 = iVar2;
  if ((DAT_0062b254 == 2) && (DAT_0062b258 == 3)) {
    iVar2 = FUN_00563750(1);
    if (iVar2 != 0) goto switchD_005597f9_default;
    FUN_00563600();
    cVar1 = FUN_004493b0();
    if (cVar1 != '\0') {
      piVar3 = (int *)FUN_00448e70();
      iVar2 = (**(code **)(*piVar3 + 0x14))();
      if (1 < iVar2) goto LAB_005597e7;
    }
    iVar2 = DAT_005d1c28;
    DAT_005d19a0 = &DAT_00623e40;
    DAT_005d19a4 = &DAT_00624100;
    DAT_005d19a8 = &DAT_006243c0;
    *(int *)(DAT_005d1c28 + 0x34) = *(int *)(DAT_005d1c28 + 0x34) + 1;
    DAT_005d19ac = &DAT_00624680;
    if ((0x77 < *(int *)(iVar2 + 0x34)) && ((DAT_006243c0 & 0x80001) != 0)) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x674),1);
      *(undefined4 *)(param_1 + 0x674) = 0;
      FUN_004316b0(10);
      *(undefined4 *)(param_1 + 0x34) = 4;
      *(undefined4 *)(param_1 + 0xfc) = 0;
      FUN_0040a670(0);
    }
    goto switchD_005597f9_default;
  }
LAB_005597e7:
  *(int *)(param_1 + 0x5d8c) = *(int *)(param_1 + 0x5d8c) + 1;
  switch(*(undefined4 *)(param_1 + 0x20)) {
  case 0:
    FUN_0055bb60(9);
    uVar7 = DAT_005c5508;
    piVar3 = (int *)(param_1 + 0x2c);
    *(undefined4 *)(param_1 + 0x34) = 4;
    *(undefined4 *)(param_1 + 0xfc) = 0;
    FUN_0040a670(uVar7);
    puVar5 = (undefined4 *)(param_1 + 0x674);
    FUN_004cb830(*(undefined4 *)(param_1 + 0x674),1);
    *puVar5 = 0;
    puVar4 = (undefined4 *)FUN_00405530(local_18,extraout_ECX,0x13,0xffffffff,extraout_ECX);
    uVar7 = *puVar4;
    *puVar5 = uVar7;
    FUN_004cb7a0(uVar7,3);
    FUN_004cb830(*puVar5,(int)(short)(*(short *)piVar3 + 0xd));
    if (DAT_005d1c14 != 6) {
      FUN_00431620(1);
      goto switchD_005597f9_caseD_1;
    }
    uVar7 = FUN_0040e510();
    FUN_0040a670(uVar7);
    FUN_004cb7a0(*puVar5,3);
    FUN_004cb7a0(*puVar5,(int)(short)(*(short *)piVar3 + 7));
    FUN_004cb7a0(*puVar5,6);
    puVar5 = (undefined4 *)FUN_004cafa0(local_18,*piVar3 + 7,extraout_ECX_00);
    FUN_004cb830(*puVar5,2);
    goto LAB_00559a14;
  case 1:
switchD_005597f9_caseD_1:
    if (6 < *(int *)(param_1 + 0x618)) {
      FUN_00431620(2);
    }
    break;
  case 2:
    *(undefined4 *)(param_1 + 0x30) = *(undefined4 *)(param_1 + 0x2c);
    iVar2 = FUN_00559b80();
    if ((iVar2 != 0) || (iVar2 = FUN_00559b80(), iVar2 != 0)) {
      FUN_004d4fa0(0xffffffff);
    }
    iVar2 = FUN_00559b80();
    if ((iVar2 != 0) || (iVar2 = FUN_00559b80(), iVar2 != 0)) {
      FUN_004d4fa0(1);
    }
    puVar5 = (undefined4 *)(param_1 + 0x674);
    if (*(int *)(param_1 + 0x30) != *(int *)(param_1 + 0x2c)) {
      FUN_004bb8d0(10,0);
      FUN_004cb7a0(*puVar5,3);
      FUN_004cb830(*puVar5,(int)(short)(*(short *)(param_1 + 0x2c) + 7));
    }
    iVar2 = FUN_00559b30();
    if (iVar2 == 0) {
      iVar2 = FUN_00559b30();
      if (iVar2 != 0) {
        FUN_004cb830(*puVar5,6);
        puVar5 = (undefined4 *)FUN_004cafa0(local_18,*(int *)(param_1 + 0x2c) + 7,extraout_ECX_01);
        FUN_004cb830(*puVar5,2);
        FUN_00431620(3);
        FUN_004bb8d0(7,0);
      }
    }
    else {
      FUN_00431620(4);
      FUN_004bb8d0(9,0);
      FUN_004cb830(*puVar5,1);
      *puVar5 = 0;
    }
    break;
  case 3:
    if (*(int *)(param_1 + 0x618) < 0xe) break;
LAB_00559a14:
    FUN_004316b0(0xb);
    FUN_00430300(*(undefined4 *)(param_1 + 0x2c));
    DAT_005c5508 = FUN_0040e510();
    FUN_004d5090();
    *(undefined4 *)(param_1 + 0xfc) = 1;
    *(undefined4 *)(param_1 + 0x34) = 0x13;
    FUN_0040a670(0);
    break;
  case 4:
    if (5 < *(int *)(param_1 + 0x618)) {
      iVar2 = FUN_0040a540();
      if (iVar2 == 2) {
        iVar6 = FUN_00414960();
        iVar2 = DAT_005d1c28;
        if (iVar6 == 3) {
          *(uint *)(DAT_005d1c28 + 0x70) = *(uint *)(DAT_005d1c28 + 0x70) & 0xffffff1f;
          *(undefined4 *)(iVar2 + 0x8c) = 0;
          uVar7 = 10;
        }
        else {
          uVar7 = 9;
        }
      }
      else {
        uVar7 = 1;
      }
      FUN_004316b0(uVar7);
      FUN_00430300(*(undefined4 *)(param_1 + 0x2c));
      DAT_005c5508 = *(undefined4 *)(param_1 + 0x2c);
      FUN_004d5040();
      *(undefined4 *)(param_1 + 0x5d8c) = 0;
    }
  }
switchD_005597f9_default:
  FUN_0046231d(&DAT_0062b018);
  ExceptionList = local_10;
  return 1;
}


// Reference 0055b555
