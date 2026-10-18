/* Entry: 0x00472a30; symbol: FUN_00472a30; body bytes: 732 */

undefined4 __fastcall FUN_00472a30(void *param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  
  if ((*(uint *)((int)param_1 + 0x3344) >> 3 & 1) == 0) {
    if (((*(uint *)((int)param_1 + 0x3344) >> 2 & 1) == 0) ||
       (iVar2 = FUN_0040ff90((int)param_1 + 0x3348), iVar2 < 0x3c)) {
      FUN_004455c0((int)DAT_005c0028);
      *(undefined4 *)((int)param_1 + 0x340) = DAT_005c5538;
      *(undefined4 *)((int)param_1 + 0x344) = DAT_005c553c;
      puVar3 = (undefined4 *)((int)param_1 + 0x214);
      puVar4 = &DAT_005c53fc;
      for (iVar2 = 0x5b; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *puVar3;
        puVar3 = puVar3 + 1;
        puVar4 = puVar4 + 1;
      }
      FUN_00477520(&DAT_005c4d40,3);
      FUN_004dda60(0x5c4d40);
      FUN_004ddf80(0x5c4d40);
      FUN_004d9db0(&DAT_005c4d40,0x17,8);
      FUN_00449e40(DAT_005c0028,0x27);
      FUN_004d9db0(&DAT_005c4d40,0x17,4);
      FUN_00449e40(DAT_005c0028,0x28);
      FUN_004d9db0(&DAT_005c4d40,0x17,4);
      FUN_004d9db0(&DAT_005c4d40,0x22,*(undefined4 *)((int)param_1 + 0x37c));
      FUN_004d9db0(&DAT_005c4d40,0x24,*(undefined4 *)((int)param_1 + 0x364));
      FUN_004d9db0(&DAT_005c4d40,0x25,*(undefined4 *)((int)param_1 + 0x368));
    }
    if (((*(uint *)((int)param_1 + 0x3344) >> 2 & 1) != 0) &&
       (iVar2 = FUN_0040ff90((int)param_1 + 0x3348), 0x1d < iVar2)) {
      *(undefined1 *)((int)param_1 + 0x331f) = 0;
    }
    if ((*(uint *)((int)param_1 + 0x3344) & 1) != 0) {
      FUN_004ddf80(0x5c4d40);
      FUN_004ddac0(0x5c4d40);
      FUN_00474880(param_1,8);
      FUN_00474880(param_1,9);
      FUN_00474880(param_1,10);
      FUN_00474880(param_1,0xb);
      FUN_004455c0((int)DAT_005c0028);
    }
    FUN_00475120((int)DAT_005c0028);
    iVar2 = FUN_0040ff90((int)param_1 + 0x3348);
    if (0 < iVar2) {
      FUN_00429420((void *)((int)param_1 + 0x3348));
      bVar1 = FUN_00423590((void *)((int)param_1 + 0x3348),0);
      if (bVar1) {
        *(undefined1 *)((int)param_1 + 0x331f) = 0xff;
        if ((*(uint *)((int)param_1 + 0x3344) >> 1 & 1) != 0) {
          *(uint *)((int)param_1 + 0x3344) = *(uint *)((int)param_1 + 0x3344) | 8;
        }
        *(undefined4 *)((int)param_1 + 0x331c) = 0xffffff;
        *(uint *)((int)param_1 + 0x3344) = *(uint *)((int)param_1 + 0x3344) & 0xfffffffd;
        *(uint *)((int)param_1 + 0x3344) = *(uint *)((int)param_1 + 0x3344) & 0xfffffffb;
      }
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    FUN_004dda60(0x5c4d40);
  }
  return 1;
}

