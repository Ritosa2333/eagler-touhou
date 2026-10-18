/* Entry: 0x004d8f80; symbol: FUN_004d8f80; body bytes: 191 */

undefined4 FUN_004d8f80(void)

{
  int *piVar1;
  
  if (DAT_005c4edc != 0) {
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xe4))(piVar1,0xab,1);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,0);
    FUN_004ddf80(0x5c4d40);
    FUN_0044c570(DAT_005c4ef4);
    FUN_004455c0(DAT_005c0028);
    FUN_0044fa10(DAT_005c4ef4,0xffffffff);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,1);
  }
  return 1;
}

