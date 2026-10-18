/* Entry: 0x004b9ef0; symbol: FUN_004b9ef0; body bytes: 1214 */

void __fastcall FUN_004b9ef0(undefined4 *param_1)

{
  bool bVar1;
  char **ppcVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  ppcVar2 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR__scalar_deleting_destructor__00570b04;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown GameTaskInf\n",ppcVar2);
  FUN_004bd920((int)param_1);
  if (DAT_005c6108 != 0) {
    FUN_0050f660(DAT_005c6108);
  }
  FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
  DAT_005c4d30 = 0;
  DAT_005c4d34 = 0;
  iVar3 = FUN_004ac0c0(0x5c4d40);
  if (iVar3 == 10) {
LAB_004b9f97:
    FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
    iVar3 = FUN_004ac0c0(0x5c4d40);
    if (iVar3 == 0x16) {
      FUN_004bdd60(&DAT_005ba568,1);
    }
    FUN_004be220(&DAT_005ba568,1);
  }
  else {
    iVar3 = FUN_004ac0c0(0x5c4d40);
    if (iVar3 == 0xb) goto LAB_004b9f97;
    iVar3 = FUN_004ac0c0(0x5c4d40);
    if (iVar3 == 4) {
      FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      std::_Adl_verify_range<char*,char_const*>((char **)" Goto Title\n",ppcVar2);
    }
    else {
      iVar3 = FUN_004ac0c0(0x5c4d40);
      if (iVar3 == 0x10) {
        FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
        std::_Adl_verify_range<char*,char_const*>((char **)" Goto Title\n",ppcVar2);
      }
      else {
        iVar3 = FUN_004ac0c0(0x5c4d40);
        if (iVar3 == 0xe) {
          FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
          std::_Adl_verify_range<char*,char_const*>((char **)" Continue\n",ppcVar2);
          iVar3 = FUN_00498f40();
          iVar3 = FUN_004bd5c0(iVar3);
          iVar4 = FUN_00498f40();
          iVar4 = FUN_00474d80(iVar4);
          if (iVar3 != iVar4) {
            FUN_004bccc0(0x5ba568);
            FUN_004bdad0(&DAT_005ba568,1);
          }
          FUN_004be220(&DAT_005ba568,1);
        }
        else {
          FUN_004ac0c0(0x5c4d40);
        }
      }
    }
  }
  uVar5 = FUN_004bd760(0x5ba568);
  if (uVar5 == 0) {
    iVar3 = FUN_004ac0c0(0x5c4d40);
    if (iVar3 != 0xf) {
      iVar3 = FUN_004ac0c0(0x5c4d40);
      if (iVar3 != 0x10) {
        FUN_004217c0(DAT_005c60fc);
      }
    }
    FUN_004217c0(DAT_005c069c);
    FUN_004217c0(DAT_005c06a0);
    FUN_004217c0(DAT_005c60bc);
    FUN_004217c0(DAT_005c06a4);
    FUN_004feff0(0);
    FUN_00485340(0);
    FUN_004c47a0(0);
    FUN_004d5be0(0);
    FUN_00510830(0);
    FUN_00513cc0();
  }
  else {
    FUN_004b64a0((int)DAT_005c06a4);
    FUN_004217c0(DAT_005c06a0);
    DAT_005c06a0 = DAT_005c069c;
    piVar6 = (int *)FUN_004bd590();
    (**(code **)(*piVar6 + 8))();
    piVar6 = (int *)FUN_00478e80(0);
    (**(code **)(*piVar6 + 8))();
    (**(code **)(*DAT_005c60fc + 8))();
    iVar3 = FUN_00498fd0(0);
    FUN_004bc220(iVar3);
  }
  uVar5 = FUN_00474e00(0x5ba568);
  if (uVar5 == 0) {
    uVar5 = FUN_004b8370(0x5ba568);
    if (uVar5 != 0) goto LAB_004ba263;
    FUN_004aa940(0);
  }
  else {
LAB_004ba263:
    FUN_00478060(0);
    FUN_004a80f0();
  }
  FUN_00437520(0);
  FUN_0049d400();
  FUN_00477fd0(0);
  FUN_00488690(0);
  piVar6 = (int *)FUN_00464230(0);
  FUN_00532f40(piVar6);
  piVar6 = (int *)FUN_00464230(0);
  (**(code **)(*piVar6 + 8))();
  piVar6 = (int *)FUN_0051b960();
  (**(code **)(*piVar6 + 8))();
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  DAT_005ba828 = 0;
  bVar1 = FUN_00488850(0x5ba568);
  if (bVar1) {
    uVar5 = FUN_00474e00(0x5ba568);
    if (uVar5 != 0) goto LAB_004ba357;
  }
  uVar5 = FUN_004bd760(0x5ba568);
  if ((uVar5 == 0) && ((DAT_005ba5d4 >> 5 & 1) == 0)) {
    FUN_004d9bc0();
    FUN_004bd960(0x5ba830);
  }
LAB_004ba357:
  FUN_004ba440(0x5ba830);
  uVar5 = FUN_00474e00(0x5ba568);
  if (uVar5 == 0) {
    DAT_005c5b20 = 0xff000000;
  }
  else {
    DAT_005c5b20 = 0;
  }
  FUN_004217c0(DAT_005c6114);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

