/* Entry: 0x004d49e0; symbol: FUN_004d49e0; body bytes: 1717 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d49e0(void *param_1)

{
  bool bVar1;
  int iVar2;
  void *pvVar3;
  float10 fVar4;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  FUN_00439330(param_1,&local_14,*(float *)((int)param_1 + 0x70),*(float *)((int)param_1 + 0x74));
  FUN_004296e0(&local_14,(float *)((int)param_1 + 0x58));
  local_c = 0;
  iVar2 = FUN_00485390(&local_14,0.0,0.0);
  if (iVar2 != 0) {
    bVar1 = false;
    iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
    if (((*(uint *)(iVar2 + 0x38) & 1) != 0) && (local_10 < 0.0)) {
      iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
      if ((*(uint *)(iVar2 + 0x38) & 0x10) == 0) {
        FUN_004545d0((float *)((int)param_1 + 0x6f8),(float *)((int)param_1 + 0x6fc),DAT_0056e1a4,
                     0.0,DAT_0056cda0,0.0,local_14,local_10,*(float *)((int)param_1 + 0x58),
                     *(float *)((int)param_1 + 0x5c));
        *(undefined4 *)((int)param_1 + 0x700) = 0;
        *(uint *)((int)param_1 + 0x704) = *(uint *)((int)param_1 + 0x70) ^ _DAT_0056d7d0;
        iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
        *(undefined4 *)((int)param_1 + 0x718) = *(undefined4 *)(iVar2 + 0x10);
        *(undefined4 *)((int)param_1 + 0x724) = 0;
        pvVar3 = (void *)FUN_0041ca90(*(int *)((int)param_1 + 0x6f0));
        FUN_004d47f0(pvVar3,0,(int)param_1 + 0x6f8);
      }
      bVar1 = true;
    }
    iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
    if (((*(uint *)(iVar2 + 0x38) & 2) != 0) && (DAT_0056d7c0 < local_10)) {
      iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
      if ((*(uint *)(iVar2 + 0x38) & 0x10) == 0) {
        FUN_004545d0((float *)((int)param_1 + 0x6f8),(float *)((int)param_1 + 0x6fc),DAT_0056e1a4,
                     DAT_0056d7c0,DAT_0056cda0,DAT_0056d7c0,local_14,local_10,
                     *(float *)((int)param_1 + 0x58),*(float *)((int)param_1 + 0x5c));
        *(undefined4 *)((int)param_1 + 0x700) = 0;
        *(uint *)((int)param_1 + 0x704) = *(uint *)((int)param_1 + 0x70) ^ _DAT_0056d7d0;
        iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
        *(undefined4 *)((int)param_1 + 0x718) = *(undefined4 *)(iVar2 + 0x10);
        *(undefined4 *)((int)param_1 + 0x724) = 0;
        pvVar3 = (void *)FUN_0041ca90(*(int *)((int)param_1 + 0x6f0));
        FUN_004d47f0(pvVar3,0,(int)param_1 + 0x6f8);
      }
      bVar1 = true;
    }
    iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
    if (((*(uint *)(iVar2 + 0x38) & 4) != 0) && (local_14 < DAT_0056fd6c / DAT_0056c8d0)) {
      iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
      if ((*(uint *)(iVar2 + 0x38) & 0x10) == 0) {
        FUN_004545d0((float *)((int)param_1 + 0x6f8),(float *)((int)param_1 + 0x6fc),
                     DAT_0056fd6c / DAT_0056c8d0,DAT_0056fa6c,DAT_0056fd6c / DAT_0056c8d0,
                     DAT_0056d7c0 + DAT_0056e050,local_14,local_10,*(float *)((int)param_1 + 0x58),
                     *(float *)((int)param_1 + 0x5c));
        *(undefined4 *)((int)param_1 + 0x700) = 0;
        fVar4 = FUN_00438540((float)(*(uint *)((int)param_1 + 0x70) ^ _DAT_0056d7d0) - DAT_0056e0f0)
        ;
        *(float *)((int)param_1 + 0x704) = (float)fVar4;
        iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
        *(undefined4 *)((int)param_1 + 0x718) = *(undefined4 *)(iVar2 + 0x10);
        *(undefined4 *)((int)param_1 + 0x724) = 0;
        pvVar3 = (void *)FUN_0041ca90(*(int *)((int)param_1 + 0x6f0));
        FUN_004d47f0(pvVar3,0,(int)param_1 + 0x6f8);
      }
      bVar1 = true;
    }
    iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
    if (((*(uint *)(iVar2 + 0x38) & 8) != 0) && (DAT_0056cda4 / DAT_0056c8d0 < local_14)) {
      iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
      if ((*(uint *)(iVar2 + 0x38) & 0x10) == 0) {
        FUN_004545d0((float *)((int)param_1 + 0x6f8),(float *)((int)param_1 + 0x6fc),
                     DAT_0056cda4 / DAT_0056c8d0,DAT_0056fa6c,DAT_0056cda4 / DAT_0056c8d0,
                     DAT_0056d7c0 + DAT_0056e050,local_14,local_10,*(float *)((int)param_1 + 0x58),
                     *(float *)((int)param_1 + 0x5c));
        *(undefined4 *)((int)param_1 + 0x700) = 0;
        fVar4 = FUN_00438540((float)(*(uint *)((int)param_1 + 0x70) ^ _DAT_0056d7d0) - DAT_0056e0f0)
        ;
        *(float *)((int)param_1 + 0x704) = (float)fVar4;
        iVar2 = FUN_0047c680((void *)((int)param_1 + 0x90),4);
        *(undefined4 *)((int)param_1 + 0x718) = *(undefined4 *)(iVar2 + 0x10);
        *(undefined4 *)((int)param_1 + 0x724) = 0;
        pvVar3 = (void *)FUN_0041ca90(*(int *)((int)param_1 + 0x6f0));
        FUN_004d47f0(pvVar3,0,(int)param_1 + 0x6f8);
      }
      bVar1 = true;
    }
    if ((bVar1) &&
       (*(uint *)((int)param_1 + 0x698) = *(uint *)((int)param_1 + 0x698) & 0xffffffbf,
       -1 < *(int *)((int)param_1 + 0x744))) {
      FUN_00426d70(&DAT_005ba830,*(int *)((int)param_1 + 0x744),0);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

