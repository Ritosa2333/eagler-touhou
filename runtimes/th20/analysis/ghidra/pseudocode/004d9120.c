/* Entry: 0x004d9120; symbol: FUN_004d9120; body bytes: 238 */

undefined4 FUN_004d9120(void)

{
  int *piVar1;
  
  if (DAT_005c4edc != 0) {
    if (DAT_005c4d30 == (code *)0x0) {
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xab,1);
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,0);
      FUN_004ddf80(0x5c4d40);
      FUN_0041dce0(&DAT_005c4d40,3);
      FUN_0044c570(DAT_005c4eec);
      FUN_0044fa10(DAT_005c4eec,0xffffffff);
      FUN_00449e40(DAT_005c0028,0x29);
      FUN_004455c0((int)DAT_005c0028);
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,1);
    }
    else {
      (*DAT_005c4d30)();
    }
  }
  return 1;
}

