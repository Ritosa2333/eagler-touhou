/* Entry: 0x0051e3c0; symbol: FUN_0051e3c0; body bytes: 2368 */

void __fastcall FUN_0051e3c0(void *param_1)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 *this;
  void *pvVar5;
  undefined4 uVar6;
  int *piVar7;
  int iVar8;
  uint local_20;
  int local_14;
  
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (DAT_005c49e8 != (undefined4 *)0x0) {
    uVar3 = FUN_0052c730((int)DAT_005c49e8);
    if (uVar3 != 0) {
      FUN_004217c0(DAT_005c49e8);
      FUN_004d9a70(0,0x575098);
      FUN_004d9b50(0,0);
    }
    goto LAB_0051ed1e;
  }
  if (*(int *)((int)param_1 + 0x18) == 1) {
    FUN_0052c420(0x5ba568);
    if (DAT_005b889c == (void *)0x0) {
      local_20 = 0;
    }
    else {
      local_20 = FUN_004495a0(DAT_005b889c,0xffff);
    }
    if (local_20 != 0) {
      FUN_0052cd80(&DAT_005ba568,0);
    }
    iVar4 = FUN_0052c2c0(0x5ba568);
    if (0x707 < iVar4) {
      FUN_0052cda0(&DAT_005ba568,1);
      FUN_0052cf10(&DAT_005ba568,0);
      iVar4 = FUN_0052c2e0(0x5ba568);
      FUN_00548610(&DAT_005c4b20,0x100,(int)(&PTR_s_demo_demo1_rpy_005b0a64)[iVar4]);
      this = FUN_0050a8e0((undefined1 (*) [16])&DAT_005c4b20);
      iVar4 = FUN_0052c2e0(0x5ba568);
      uVar3 = iVar4 + 1U & 0x80000003;
      if ((int)uVar3 < 0) {
        uVar3 = (uVar3 - 1 | 0xfffffffc) + 1;
      }
      FUN_0052cdd0(&DAT_005ba568,uVar3);
      local_14 = 0;
      while ((local_14 < 8 && (iVar4 = FUN_00488770(this,local_14), iVar4 == 0))) {
        local_14 = local_14 + 1;
      }
      FUN_004be360(&DAT_005ba568,local_14);
      FUN_004be250(&DAT_005c4d40,0xd);
      iVar4 = FUN_00421810((int)this);
      FUN_0050a3e0(&DAT_005ba568,*(undefined4 *)(iVar4 + 0xd8));
      pvVar5 = (void *)FUN_00464080(0);
      FUN_00412dc0(pvVar5,*(undefined4 *)(iVar4 + 0xdc));
      pvVar5 = (void *)FUN_00464080(0);
      FUN_0041de50(pvVar5,*(undefined4 *)(iVar4 + 0xe0));
      pvVar5 = (void *)FUN_00464080(0);
      FUN_00412da0(pvVar5,*(undefined4 *)(iVar4 + 0xe4));
      pvVar5 = (void *)FUN_00464080(0);
      FUN_0041de90(pvVar5,*(undefined4 *)(iVar4 + 0xe8));
      uVar6 = FUN_004640a0(0x5ba568);
      FUN_0052ce30(&DAT_005ba568,uVar6);
      FUN_004be1b0(&DAT_005ba568,*(undefined4 *)(iVar4 + 0xf0));
      FUN_004217c0(this);
      DAT_005c6128 = 1;
      FUN_0052cd80(&DAT_005ba568,0);
    }
  }
  if (((*(uint *)((int)param_1 + 0x58d4) & 1) != 0) &&
     (((*(int *)((int)param_1 + 0x58d0) = *(int *)((int)param_1 + 0x58d0) + 1, DAT_005c6128 == 1 &&
       (0x1d < *(int *)((int)param_1 + 0x58d0))) || (DAT_005c6128 != 1)))) {
    FUN_004d9bc0();
    FUN_004bd960(0x5ba830);
    FUN_004d9a70(0,0x575098);
    FUN_004d9b50(0,0);
    *(uint *)((int)param_1 + 0x58d4) = *(uint *)((int)param_1 + 0x58d4) & 0xfffffffe;
    *(undefined4 *)((int)param_1 + 0x58d0) = 0;
  }
  switch(*(undefined4 *)((int)param_1 + 0x18)) {
  case 0:
    DAT_005ba518 = 0;
    DAT_005afcfc = 0xffffffff;
    uVar6 = FUN_0051e170();
    FUN_004d9940(&DAT_005ba4a8,uVar6);
    uVar6 = FUN_0051e170();
    FUN_004d9940(&DAT_005ba4c4,uVar6);
    piVar7 = (int *)FUN_0051b960();
    (**(code **)(*piVar7 + 4))();
    FUN_004a2800(*(void **)((int)param_1 + 0x5924));
    *(undefined4 *)((int)param_1 + 0x5924) = 0;
    FUN_004863f0(DAT_005c0028,5,'\0');
    FUN_004863f0(DAT_005c0028,7,'\0');
    FUN_004863f0(DAT_005c0028,2,'\0');
    FUN_004863f0(DAT_005c0028,0,'\0');
    FUN_004a05d0(DAT_005c0698);
    FUN_0052c030((int)param_1);
    if (DAT_005c6128 != 3) {
      uVar3 = FUN_004b8390(0x5ba568);
      if (uVar3 == 0) {
        *(uint *)((int)param_1 + 0x58d4) = *(uint *)((int)param_1 + 0x58d4) | 1;
        *(undefined4 *)((int)param_1 + 0x58d0) = 0;
      }
      else {
        *(uint *)((int)param_1 + 0x58d4) = *(uint *)((int)param_1 + 0x58d4) & 0xfffffffe;
        uVar6 = FUN_0052c3e0(0x5ba568);
        FUN_004be1b0(&DAT_005ba568,uVar6);
      }
      FUN_0052cda0(&DAT_005ba568,0);
      if (DAT_005c6128 == 0) {
        *(uint *)((int)param_1 + 0x58d4) = *(uint *)((int)param_1 + 0x58d4) | 2;
      }
      else {
        *(uint *)((int)param_1 + 0x58d4) = *(uint *)((int)param_1 + 0x58d4) & 0xfffffffd;
      }
      if (DAT_005c6128 == 0) {
        FUN_0052ced0(param_1,1);
        DAT_005c6128 = 1;
      }
      else if (DAT_005c6128 == 1) {
        iVar4 = FUN_004640a0(0x5ba568);
        if (iVar4 == 4) {
          FUN_004bed50((void *)((int)param_1 + 0x24),1);
        }
        FUN_0052ced0(param_1,1);
      }
      else {
        if (DAT_005c6128 == 2) {
          FUN_0050a4a0(&DAT_005ba568,0);
          FUN_0049c4e0((void *)((int)param_1 + 0x24),10);
          FUN_004bed50((void *)((int)param_1 + 0x24),4);
          FUN_004c60b0((int)param_1 + 0x24);
          FUN_0052ced0(param_1,0xc);
          DAT_005c6128 = 1;
          FUN_00412d80(*(int *)((int)param_1 + 0xc));
          goto switchD_0051e731_caseD_c;
        }
        if (DAT_005c6128 == 6) {
          FUN_0050a4a0(&DAT_005ba568,0);
          FUN_0049c4e0((void *)((int)param_1 + 0x24),10);
          FUN_004bed50((void *)((int)param_1 + 0x24),5);
          FUN_004c60b0((int)param_1 + 0x24);
          FUN_0049c4e0((void *)((int)param_1 + 0x24),10);
          FUN_004bed50((void *)((int)param_1 + 0x24),2);
          FUN_004c60b0((int)param_1 + 0x24);
          FUN_0050a3e0(&DAT_005ba568,DAT_005c6134);
          FUN_0052cf40(&DAT_005ba568,DAT_005c6138);
          iVar4 = FUN_004b8340(0x5ba568);
          iVar8 = FUN_00464080(0);
          iVar8 = FUN_00412730(iVar8);
          FUN_0051c920(DAT_005c6108,0,iVar8,iVar4);
          __aligned_offset_recalloc((void *)0x9,0x1e,0,0,0);
          FUN_0052cf80(param_1,0);
          FUN_0052cc90(param_1,0);
          FUN_0052ced0(param_1,0x17);
          DAT_005c6128 = 1;
          FUN_00412d80(*(int *)((int)param_1 + 0xc));
          goto switchD_0051e731_caseD_17;
        }
        if (DAT_005c6128 == 5) {
          __aligned_offset_recalloc((void *)0x9,0x1e,0,0,0);
          FUN_0052cf80(param_1,0);
          FUN_0052cc90(param_1,0);
          FUN_0052ced0(param_1,6);
          FUN_00412d80(*(int *)((int)param_1 + 0xc));
          goto switchD_0051e731_caseD_6;
        }
        if (DAT_005c6128 == 4) {
          __aligned_offset_recalloc((void *)0x9,0x1e,0,0,0);
          FUN_0052cf80(param_1,0);
          FUN_0052cc90(param_1,0);
          FUN_0052ced0(param_1,5);
          FUN_00412d80(*(int *)((int)param_1 + 0xc));
          goto switchD_0051e731_caseD_5;
        }
      }
      goto switchD_0051e731_caseD_1;
    }
    FUN_0049c4e0((void *)((int)param_1 + 0x24),10);
    FUN_004bed50((void *)((int)param_1 + 0x24),0);
    FUN_004c60b0((int)param_1 + 0x24);
    FUN_0052ced0(param_1,0xf);
    DAT_005c6128 = 1;
    FUN_00412d80(*(int *)((int)param_1 + 0xc));
  case 0xf:
    FUN_005223e0(param_1);
    break;
  case 1:
switchD_0051e731_caseD_1:
    FUN_00412d80(*(int *)((int)param_1 + 0xc));
    FUN_00529e80(param_1);
    break;
  case 2:
    FUN_004a0fb0(&DAT_005c4d40,3);
    FUN_004d9bc0();
    break;
  case 3:
    FUN_00520fb0(param_1);
    break;
  case 5:
switchD_0051e731_caseD_5:
    FUN_00521bc0();
    break;
  case 6:
switchD_0051e731_caseD_6:
    FUN_00521060();
    break;
  case 7:
    FUN_0052b8e0();
    break;
  case 8:
    FUN_00529430(param_1);
    break;
  case 9:
    FUN_004d9bc0();
    break;
  case 10:
    FUN_005265a0(param_1);
    break;
  case 0xb:
    FUN_00524c10(param_1);
    break;
  case 0xc:
switchD_0051e731_caseD_c:
    FUN_00523440(param_1);
    break;
  case 0xd:
    FUN_004d9bc0();
    break;
  case 0xe:
    FUN_005205d0(param_1);
    break;
  case 0x10:
    FUN_00526a90(param_1);
    break;
  case 0x11:
    FUN_005204c0(param_1);
    break;
  case 0x12:
    FUN_00528fc0(param_1);
    break;
  case 0x13:
    FUN_005284f0(param_1);
    break;
  case 0x14:
    FUN_00527f00(param_1);
    break;
  case 0x17:
switchD_0051e731_caseD_17:
    FUN_0052a8a0(param_1);
    break;
  case 0x18:
    FUN_00412540();
  }
  FUN_00423540((undefined4 *)((int)param_1 + 0x154));
  bVar1 = FUN_00461070((void *)((int)param_1 + 0x5904),0);
  if (bVar1) {
    FUN_00429420((void *)((int)param_1 + 0x5904));
  }
  bVar1 = FUN_00461070((void *)((int)param_1 + 0x5914),0);
  if (bVar1) {
    FUN_00429420((void *)((int)param_1 + 0x5914));
  }
LAB_0051ed1e:
  FUN_005429ee(uVar2 ^ (uint)&stack0xfffffffc);
  return;
}

