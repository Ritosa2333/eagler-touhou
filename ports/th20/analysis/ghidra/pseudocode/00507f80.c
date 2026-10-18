/* Entry: 0x00507f80; symbol: FUN_00507f80; body bytes: 606 */

undefined4 __fastcall FUN_00507f80(void *param_1)

{
  bool bVar1;
  void *pvVar2;
  undefined3 extraout_var;
  int iVar3;
  float10 fVar4;
  undefined1 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  if (DAT_005ba828 != 0) {
    DAT_005b8b50 = DAT_005b8b4c;
    DAT_005b8b4c = DAT_005b88b0 & 0xffff;
    FUN_0050a4d0(0x5b88b0);
    if ((DAT_005c4f8c >> 8 & 1) != 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_14 = 0;
      }
      else {
        local_14 = FUN_004973a0(DAT_005b889c,1);
      }
      if (local_14 != 0) {
        if (DAT_005b889c == (void *)0x0) {
          local_18 = 0;
        }
        else {
          local_18 = FUN_004b5b60(DAT_005b889c,0);
        }
        if (9 < local_18) {
          DAT_005b8b4c = DAT_005b8b4c | 8;
        }
      }
    }
    if (-1 < *(int *)((int)param_1 + 0x250)) {
      if (*(int *)((int)param_1 + 0x250) % 0x1e == 0) {
        pvVar2 = (void *)FUN_0040c300(*(undefined4 **)((int)param_1 + 0xe0));
        fVar4 = FUN_0049be00(DAT_005c4a00);
        if ((float)fVar4 + DAT_0056e0ec < DAT_0056cda0) {
          fVar4 = FUN_0049be00(DAT_005c4a00);
          local_1c = (undefined1)(int)((float)fVar4 + DAT_0056e0ec);
        }
        else {
          local_1c = 0xff;
        }
        FUN_00509ee0(pvVar2,local_1c);
      }
      pvVar2 = (void *)FUN_0040c300(*(undefined4 **)((int)param_1 + 0xe0));
      bVar1 = FUN_00509e40(pvVar2,(undefined2)DAT_005b8b4c,DAT_005b8b58,DAT_005b8b5c);
      if (CONCAT31(extraout_var,bVar1) != 0) {
        iVar3 = FUN_00474d60(0x5ba568);
        iVar3 = FUN_0050a2e0(param_1,iVar3);
        *(int *)((int)param_1 + 0xe0) = iVar3;
      }
      *(int *)((int)param_1 + 0x250) = *(int *)((int)param_1 + 0x250) + 1;
    }
  }
  return 1;
}

