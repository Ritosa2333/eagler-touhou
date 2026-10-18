/* Entry: 0x00472d10; symbol: FUN_00472d10; body bytes: 1063 */

void __fastcall FUN_00472d10(void *param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(uint *)((int)param_1 + 0x3344) >> 3 & 1) == 0) {
    if (((*(uint *)((int)param_1 + 0x3344) >> 2 & 1) == 0) ||
       (iVar1 = FUN_0040ff90((int)param_1 + 0x3348), iVar1 < 0x3c)) {
      FUN_004455c0((int)DAT_005c0028);
      *(undefined4 *)((int)param_1 + 0x340) = DAT_005c5538;
      *(undefined4 *)((int)param_1 + 0x344) = DAT_005c553c;
      puVar3 = (undefined4 *)((int)param_1 + 0x214);
      puVar4 = &DAT_005c53fc;
      for (iVar1 = 0x5b; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar4 = *puVar3;
        puVar3 = puVar3 + 1;
        puVar4 = puVar4 + 1;
      }
      FUN_00477520(&DAT_005c4d40,3);
      FUN_004ddfe0(0x5c4d40);
      FUN_004d9db0(&DAT_005c4d40,0x17,4);
      FUN_004d9db0(&DAT_005c4d40,0x22,*(undefined4 *)((int)param_1 + 0x37c));
      FUN_004d9db0(&DAT_005c4d40,0x24,*(undefined4 *)((int)param_1 + 0x364));
      FUN_004d9db0(&DAT_005c4d40,0x25,*(undefined4 *)((int)param_1 + 0x368));
      if (((*(uint *)((int)param_1 + 0x3344) >> 2 & 1) == 0) ||
         (0x21 < *(int *)((int)param_1 + 0x335c))) {
        local_18 = DAT_005c54dc;
        local_14 = DAT_005c54e0;
        local_10 = DAT_005c54dc + DAT_005c54e4;
        local_c = DAT_005c54e0 + DAT_005c54e8;
        piVar2 = (int *)FUN_00412730(0x5c4d40);
        (**(code **)(*piVar2 + 0xac))
                  (piVar2,1,&local_18,3,*(undefined4 *)((int)param_1 + 0x37c),DAT_0056c8cc,0);
      }
      else {
        local_28 = DAT_005c54dc;
        local_24 = DAT_005c54e0;
        local_20 = DAT_005c54dc + DAT_005c54e4;
        local_1c = DAT_005c54e0 + DAT_005c54e8;
        piVar2 = (int *)FUN_00412730(0x5c4d40);
        (**(code **)(*piVar2 + 0xac))(piVar2,1,&local_28,3,0xff000000,DAT_0056c8cc,0);
      }
    }
    if ((*(uint *)((int)param_1 + 0x3344) >> 2 & 1) != 0) {
      iVar1 = FUN_0040ff90((int)param_1 + 0x3348);
      if (iVar1 < 0x1e) {
        FUN_004250c0(3,0x1e,0,0,0,10);
        *(uint *)((int)param_1 + 0x3344) = *(uint *)((int)param_1 + 0x3344) | 1;
        FUN_00423520((void *)((int)param_1 + 0x3348),1);
      }
      else {
        *(undefined1 *)((int)param_1 + 0x331f) = 0;
        *(uint *)((int)param_1 + 0x3344) = *(uint *)((int)param_1 + 0x3344) & 0xfffffffe;
      }
    }
    if (*(char *)((int)param_1 + 0x331f) != '\0') {
      FUN_00477640(DAT_005c0028,*(undefined4 *)((int)param_1 + 0x331c));
      *(undefined1 *)((int)param_1 + 0x331f) = 0;
    }
    *(undefined4 *)((int)param_1 + 0x3338) = 0;
    *(undefined4 *)((int)param_1 + 0x333c) = 0;
    *(undefined4 *)((int)param_1 + 0x3340) = 0;
    if ((*(uint *)((int)param_1 + 0x3344) & 1) != 0) {
      FUN_004ddac0(0x5c4d40);
      FUN_00474880(param_1,0);
      FUN_00474880(param_1,1);
      FUN_00474880(param_1,2);
      FUN_00474880(param_1,3);
      FUN_00474880(param_1,4);
      FUN_00474880(param_1,5);
      FUN_00474880(param_1,6);
      FUN_00474880(param_1,7);
      FUN_004455c0((int)DAT_005c0028);
    }
    FUN_00475120((int)DAT_005c0028);
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

