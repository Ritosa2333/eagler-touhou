/* Entry: 0x004872d0; symbol: FUN_004872d0; body bytes: 645 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004872d0(void *param_1)

{
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  undefined3 extraout_var;
  float10 fVar4;
  undefined4 local_c;
  
  if ((*(uint *)((int)param_1 + 0x78) & 1) == 0) {
    if ((*(uint *)((int)param_1 + 0x78) >> 6 & 1) != 0) {
      *(undefined4 *)((int)param_1 + 0x90) = *(undefined4 *)((int)param_1 + 0x8c);
      fVar4 = (float10)FUN_0041cb10();
      *(double *)((int)param_1 + 0xa0) = (double)fVar4 - *(double *)((int)param_1 + 0x98);
      fVar4 = (float10)FUN_0054ffa0((uint)*(undefined8 *)((int)param_1 + 0xa0),
                                    (int)((ulonglong)*(undefined8 *)((int)param_1 + 0xa0) >> 0x20),
                                    SUB84(DAT_0056fe60,0),(int)((ulonglong)DAT_0056fe60 >> 0x20));
      *(double *)((int)param_1 + 0xa0) = *(double *)((int)param_1 + 0xa0) - (double)fVar4;
      if (DAT_0056fe60 / _DAT_0056fe68 <= (double)fVar4) {
        *(double *)((int)param_1 + 0xa0) = *(double *)((int)param_1 + 0xa0) + DAT_0056fe60;
      }
      FUN_00550130(*(double *)((int)param_1 + 0xa0));
      local_c = thunk_FUN_00543320();
      if (999 < (int)local_c) {
        local_c = 999;
      }
      fVar4 = FUN_00550130(*(double *)((int)param_1 + 0xa0));
      FUN_00488990(param_1,local_c,
                   (int)((*(double *)((int)param_1 + 0xa0) - (double)fVar4) * _DAT_0056fe70));
      *(undefined8 *)((int)param_1 + 0xa0) = 0;
      *(uint *)((int)param_1 + 0x78) = *(uint *)((int)param_1 + 0x78) & 0xffffffbf;
      iVar3 = FUN_00488830(DAT_005ba828);
      if (iVar3 == 0) {
        uVar1 = *(undefined4 *)((int)param_1 + 0xa8);
        iVar3 = FUN_00474d60(0x5ba568);
        iVar3 = FUN_00488740(DAT_005c60fc,iVar3);
        *(undefined4 *)(iVar3 + 0x1c + *(int *)((int)param_1 + 0x88) * 4) = uVar1;
      }
      else {
        iVar3 = FUN_00474d60(0x5ba568);
        iVar3 = FUN_00488770(DAT_005c60fc,iVar3);
        *(undefined4 *)((int)param_1 + 0xa8) =
             *(undefined4 *)(iVar3 + 0x1c + *(int *)((int)param_1 + 0x88) * 4);
        bVar2 = FUN_004885e0((int)param_1);
        if (CONCAT31(extraout_var,bVar2) != 0) {
          FUN_00488990(param_1,999,99);
        }
      }
      *(int *)((int)param_1 + 0x88) = *(int *)((int)param_1 + 0x88) + 1;
    }
  }
  else if ((*(uint *)((int)param_1 + 0x78) >> 6 & 1) == 0) {
    fVar4 = (float10)FUN_0041cb10();
    *(double *)((int)param_1 + 0x98) = (double)fVar4;
    *(uint *)((int)param_1 + 0x78) = *(uint *)((int)param_1 + 0x78) | 0x40;
  }
  return;
}

