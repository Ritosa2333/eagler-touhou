/* Entry: 0x004d8e90; symbol: FUN_004d8e90; body bytes: 225 */

undefined4 FUN_004d8e90(void)

{
  int *piVar1;
  
  if (DAT_005c4edc != 0) {
    if (DAT_005c4d34 == (code *)0x0) {
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xab,1);
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,0);
      FUN_004ddf80(0x5c4d40);
      FUN_0041dce0(&DAT_005c4d40,3);
      FUN_0044c570(DAT_005c4ef0);
      FUN_0044fa10(DAT_005c4ef0,0xffffffff);
      FUN_004455c0(DAT_005c0028);
      piVar1 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,1);
    }
    else {
      (*DAT_005c4d34)();
    }
  }
  return 1;
}

