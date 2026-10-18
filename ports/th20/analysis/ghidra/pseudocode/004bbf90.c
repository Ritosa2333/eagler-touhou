/* Entry: 0x004bbf90; symbol: FUN_004bbf90; body bytes: 656 */

void __fastcall FUN_004bbf90(int param_1)

{
  void *pvVar1;
  int iVar2;
  int *piVar3;
  undefined4 local_5c [21];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(uint *)(param_1 + 0xe8) >> 0xc & 1) != 0) {
    *(uint *)(param_1 + 0xe8) = *(uint *)(param_1 + 0xe8) & 0xffffefff;
    FUN_00478e80(0);
    FUN_00483ad0();
    pvVar1 = (void *)FUN_00460830(0);
    FUN_004fb450(pvVar1);
    iVar2 = FUN_00498fd0(0);
    FUN_004bc220(iVar2);
    FUN_00478060(0);
    FUN_004a80f0();
    iVar2 = FUN_00478ee0(0);
    FUN_004bc480(iVar2);
    (**(code **)(*DAT_005c60fc + 4))();
    FUN_0047bb30(local_5c);
    pvVar1 = (void *)FUN_00478060(0);
    FUN_004a8920(pvVar1,"main",local_5c,0);
    piVar3 = (int *)FUN_004bd560();
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_004bd590();
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00460830(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00478e80(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00478060(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00498fd0(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00478ee0(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00437520(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00478040(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_004bd5a0(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00478ea0(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_00464230(0);
    (**(code **)(*piVar3 + 4))();
    piVar3 = (int *)FUN_0051b960();
    (**(code **)(*piVar3 + 4))();
    FUN_004d9bc0();
    FUN_004d9b50(0,*(int *)(DAT_005c6110 + 0x58));
    FUN_004a05d0(DAT_005c0698);
    FUN_00423520((void *)(param_1 + 0x10),0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

