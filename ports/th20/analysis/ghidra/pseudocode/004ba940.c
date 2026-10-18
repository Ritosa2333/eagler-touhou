/* Entry: 0x004ba940; symbol: FUN_004ba940; body bytes: 1013 */

void __fastcall FUN_004ba940(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  int *piVar4;
  int local_64;
  undefined4 local_5c [21];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(uint *)(param_1 + 0xe8) = *(uint *)(param_1 + 0xe8) | 0x4000000;
  if ((*(uint *)(param_1 + 0xe8) >> 3 & 1) == 0) {
    (**(code **)(*DAT_005c069c + 4))();
    if (DAT_005c06a0 == 0) {
      *(uint *)(param_1 + 0xe8) = *(uint *)(param_1 + 0xe8) & 0xffffefff;
      FUN_00478e80(0);
      FUN_00483ad0();
      pvVar2 = (void *)FUN_00460830(0);
      FUN_004fb450(pvVar2);
      iVar3 = FUN_00498fd0(0);
      FUN_004bc220(iVar3);
      FUN_00478060(0);
      FUN_004a80f0();
      iVar3 = FUN_00478ee0(0);
      FUN_004bc480(iVar3);
      pvVar2 = (void *)FUN_00498f40();
      FUN_004bdb80(pvVar2,0);
      pvVar2 = (void *)FUN_00498f40();
      FUN_004b8830(pvVar2,0);
      if (DAT_005c60fc != (int *)0x0) {
        (**(code **)(*DAT_005c60fc + 4))();
      }
      FUN_0047bb30(local_5c);
      pvVar2 = (void *)FUN_00478060(0);
      FUN_004a8920(pvVar2,"main",local_5c,0);
      piVar4 = (int *)FUN_004bd560();
      (**(code **)(*piVar4 + 4))();
      piVar4 = (int *)FUN_004bd590();
      (**(code **)(*piVar4 + 4))();
      for (local_64 = 0; local_64 < 1; local_64 = local_64 + 1) {
        piVar4 = (int *)FUN_00478ec0(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00460830(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00478e80(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00478060(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00498fd0(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00478ee0(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00478040(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_004bd5a0(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00478ea0(0);
        (**(code **)(*piVar4 + 4))();
        piVar4 = (int *)FUN_00437520(0);
        (**(code **)(*piVar4 + 4))();
      }
      bVar1 = FUN_00488850(0x5ba568);
      if ((!bVar1) && ((DAT_005ba5d4 >> 5 & 1) == 0)) {
        FUN_004d9b50(0,*(int *)(DAT_005c6110 + 0x58));
      }
      FUN_004be220(&DAT_005ba568,0);
      FUN_004bdd60(&DAT_005ba568,0);
      FUN_00479040((void *)(DAT_005c0698 + 0x1a248));
      FUN_004a05d0(DAT_005c0698);
      FUN_00479040(&DAT_005c4f04);
      piVar4 = (int *)FUN_00464230(0);
      (**(code **)(*piVar4 + 4))();
      piVar4 = (int *)FUN_0051b960();
      (**(code **)(*piVar4 + 4))();
    }
    else {
      FUN_00477910(DAT_005c06a0);
      FUN_004778e0((int)DAT_005c069c);
      *(uint *)(param_1 + 0xe8) = *(uint *)(param_1 + 0xe8) | 0x1000;
      FUN_004bd8f0(DAT_005c06a4);
    }
  }
  else {
    FUN_004d9e30((char **)&DAT_005c4d40);
    FUN_004a0fb0(&DAT_005c4d40,3);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

