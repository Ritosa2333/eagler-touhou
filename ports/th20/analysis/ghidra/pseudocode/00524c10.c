/* Entry: 0x00524c10; symbol: FUN_00524c10; body bytes: 3019 */

undefined4 __fastcall FUN_00524c10(void *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  void *pvVar3;
  int iVar4;
  uint local_3c;
  uint local_38;
  uint local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  byte local_9;
  
  switch(*(undefined4 *)((int)param_1 + 0x20)) {
  case 0:
    pvVar3 = (void *)FUN_0051b960();
    FUN_0051a0b0(pvVar3,2);
    pvVar3 = (void *)FUN_0051b960();
    FUN_0052ce80(pvVar3,0);
    uVar2 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24));
    *(undefined4 *)((int)param_1 + 0x58f8) = uVar2;
    if (*(int *)((int)param_1 + 0x58f8) == 1) {
      iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
      iVar4 = FUN_004886b0(iVar4);
      FUN_0049c4e0((void *)((int)param_1 + 0x108),(iVar4 + 9) / 10);
      FUN_004bed50((void *)((int)param_1 + 0x108),0);
      FUN_004bfbe0((void *)((int)param_1 + 0x108),1);
    }
    FUN_0049c4e0((void *)((int)param_1 + 0x24),0x10);
    FUN_004bed50((void *)((int)param_1 + 0x24),0);
    FUN_0049c4e0((void *)((int)param_1 + 0xbc),5);
    FUN_004bed50((void *)((int)param_1 + 0xbc),1);
    FUN_004bfbe0((void *)((int)param_1 + 0xbc),1);
    FUN_0052ce50(param_1,1);
    FUN_0052cf80(param_1,0x48);
    iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
    FUN_0052cf80(param_1,iVar4 + 0x40);
    FUN_0052cf80(param_1,0x49);
    FUN_0052cf80(param_1,0x4a);
    FUN_0052cf80(param_1,0x4b);
    FUN_0052cf80(param_1,0x4c);
    FUN_0052cf80(param_1,0x45);
    FUN_0052cf80(param_1,0x46);
    FUN_0052cf80(param_1,0x47);
    FUN_0052cf80(param_1,0x27);
  case 1:
    bVar1 = FUN_00461070((void *)((int)param_1 + 0x154),6);
    if (bVar1) {
      FUN_0052ce50(param_1,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    FUN_004bfc00((undefined4 *)((int)param_1 + 0xbc));
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x108));
    if (DAT_005b889c == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_24 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_24 != 0) {
      FUN_004bf040((void *)((int)param_1 + 0xbc));
      FUN_0052cbf0(param_1,0x4b);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_28 = 0;
    }
    else {
      local_28 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_28 != 0) {
      FUN_004bf020((void *)((int)param_1 + 0xbc));
      FUN_0052cbf0(param_1,0x4c);
    }
    bVar1 = FUN_0045d030((int *)((int)param_1 + 0xbc));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      iVar4 = FUN_0040ff90((int)param_1 + 0xbc);
      FUN_0052cc30(param_1,iVar4 + 0x40);
      iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
      FUN_0052cf80(param_1,iVar4 + 0x40);
      if (*(int *)((int)param_1 + 0x58f8) == 1) {
        FUN_004bed50((void *)((int)param_1 + 0x108),0);
        FUN_00412540();
      }
      iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
      iVar4 = FUN_004886b0(iVar4);
      FUN_0049c4e0((void *)((int)param_1 + 0x108),(iVar4 + 9) / 10);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_2c = 0;
    }
    else {
      local_2c = FUN_0041a280(DAT_005b889c,0x40);
    }
    if (local_2c != 0) {
      FUN_004bf040((void *)((int)param_1 + 0x24));
      FUN_0052cbf0(param_1,0x49);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_30 = 0;
    }
    else {
      local_30 = FUN_0041a280(DAT_005b889c,0x80);
    }
    if (local_30 != 0) {
      FUN_004bf020((void *)((int)param_1 + 0x24));
      FUN_0052cbf0(param_1,0x4a);
    }
    bVar1 = FUN_0045d030((int *)((int)param_1 + 0x24));
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      pvVar3 = (void *)FUN_0051b960();
      iVar4 = FUN_0051e0c0((void *)((int)param_1 + 0x24),8);
      FUN_0052ce80(pvVar3,iVar4);
      iVar4 = FUN_0051e0a0((void *)((int)param_1 + 0x24),2);
      FUN_0052cd10(param_1,0x48,iVar4 + 0x25);
      FUN_00423520((void *)((int)param_1 + 0x154),0);
      if (*(int *)((int)param_1 + 0x58f8) == 1) {
        FUN_00412540();
      }
    }
    if (*(int *)((int)param_1 + 0x58f8) == 1) {
      if (DAT_005b889c == (void *)0x0) {
        local_34 = 0;
      }
      else {
        local_34 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_34 != 0) {
        FUN_004bf020((void *)((int)param_1 + 0x108));
        FUN_00412540();
        FUN_00426d70(&DAT_005ba830,7,0);
      }
    }
    iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
    if ((iVar4 == 3) && (iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24)), iVar4 == 7)) {
      if (DAT_005b889c == (void *)0x0) {
        local_38 = 0;
      }
      else {
        local_38 = FUN_00419c00(DAT_005b889c,0x8010f);
      }
      if (local_38 != 0) {
        DAT_005c6548 = 0;
        DAT_005c654c = 0;
      }
      FUN_00543e20((uint *)&DAT_005c6348,(uint *)&DAT_005c6448,0x100);
      local_1c = FUN_0051f310(&DAT_005c6448);
      if (local_1c == 2) {
        _memset(&DAT_005c6248,0,0x100);
        DAT_005c6266 = DAT_005c6489;
        DAT_005c6278 = DAT_005c648a;
        DAT_005c6276 = DAT_005c648b;
        DAT_005c6268 = DAT_005c648c;
        DAT_005c625a = DAT_005c648d;
        DAT_005c6269 = DAT_005c648e;
        DAT_005c626a = DAT_005c648f;
        DAT_005c626b = DAT_005c6490;
        DAT_005c625f = DAT_005c6491;
        DAT_005c626c = DAT_005c6492;
        DAT_005c626d = DAT_005c6493;
        DAT_005c626e = DAT_005c6494;
        DAT_005c627a = DAT_005c6495;
        DAT_005c6279 = DAT_005c6496;
        DAT_005c6260 = DAT_005c6497;
        DAT_005c6261 = DAT_005c6498;
        DAT_005c6258 = DAT_005c6499;
        DAT_005c625b = DAT_005c649a;
        DAT_005c6267 = DAT_005c649b;
        DAT_005c625c = DAT_005c649c;
        DAT_005c625e = DAT_005c649d;
        DAT_005c6277 = DAT_005c649e;
        DAT_005c6259 = DAT_005c649f;
        DAT_005c6275 = DAT_005c64a0;
        DAT_005c625d = DAT_005c64a1;
        DAT_005c6274 = DAT_005c64a2;
        FUN_00543e20((uint *)&DAT_005c6448,(uint *)&DAT_005c6248,0x100);
        local_1c = 1;
      }
      if (local_1c == 1) {
        for (local_10 = 0; local_10 < 0x100; local_10 = local_10 + 1) {
          (&DAT_005c6248)[local_10] =
               ((&DAT_005c6448)[local_10] ^ (&DAT_005c6348)[local_10]) & (&DAT_005c6448)[local_10];
        }
        if (DAT_005c6548 < 0xc) {
          if (((&DAT_005c6248)[(&DAT_00575298)[DAT_005c6548]] & 0x80) == 0) {
            local_9 = 0;
            for (local_14 = 0; local_14 < 0x39; local_14 = local_14 + 1) {
              local_9 = local_9 | (&DAT_005c6248)[local_14];
            }
            if ((local_9 & 0x80) != 0) {
              DAT_005c6548 = 0;
            }
          }
          else {
            DAT_005c6548 = DAT_005c6548 + 1;
            DAT_005c654c = 0;
          }
        }
        else {
          FUN_0051e1c0(DAT_005c6108);
          FUN_00426d70(&DAT_005ba830,0x11,0);
          DAT_005c6548 = 0;
        }
      }
      DAT_005c654c = DAT_005c654c + 1;
      if (300 < DAT_005c654c) {
        DAT_005c6548 = 0;
        DAT_005c654c = 0;
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_3c = 0;
    }
    else {
      local_3c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_3c != 0) {
      FUN_0052ce50(param_1,3);
      FUN_00426d70(&DAT_005ba830,9,0);
      iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0xbc));
      FUN_0052cc30(param_1,iVar4 + 0x40);
      FUN_0052cc30(param_1,0x49);
      FUN_0052cc30(param_1,0x4a);
      FUN_0052cc30(param_1,0x4b);
      FUN_0052cc30(param_1,0x4c);
      FUN_0052cc30(param_1,0x45);
      FUN_0052cc30(param_1,0x46);
      FUN_0052cc30(param_1,0x47);
      FUN_0052cc30(param_1,0x4d);
      FUN_0052cc30(param_1,0x48);
      for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
        pvVar3 = (void *)FUN_00414580((void *)((int)param_1 + 0x394),local_18);
        FUN_00479040(pvVar3);
      }
    }
    break;
  case 3:
    bVar1 = FUN_004235c0((void *)((int)param_1 + 0x154),6);
    if (bVar1) {
      iVar4 = FUN_0051b960();
      FUN_00519840(iVar4);
      FUN_0052cc30(param_1,0x27);
      FUN_0052ced0(param_1,10);
      FUN_004c6010((undefined4 *)((int)param_1 + 0x24));
    }
  }
  return 1;
}

