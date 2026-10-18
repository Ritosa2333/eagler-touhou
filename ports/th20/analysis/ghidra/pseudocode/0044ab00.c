/* Entry: 0x0044ab00; symbol: FUN_0044ab00; body bytes: 284 */

void __fastcall FUN_0044ab00(int *param_1)

{
  int local_c;
  
  if (param_1[0xf] != 0) {
    FUN_0044fd30(DAT_005c0028,param_1,'\0');
    for (local_c = 0; local_c < param_1[0x11]; local_c = local_c + 1) {
      FUN_0044c510(DAT_005c0028,(int *)(local_c * 0x18 + param_1[0x16]));
    }
    FUN_0040d840((LPVOID)param_1[0x16]);
    param_1[0x16] = 0;
    FUN_0040d840((LPVOID)param_1[0x14]);
    param_1[0x14] = 0;
    FUN_0040d840((LPVOID)param_1[0x15]);
    param_1[0x15] = 0;
    if (param_1[0x1b] != 0) {
      FUN_0041f670((LPVOID)param_1[0x1b]);
      param_1[0x1b] = 0;
    }
    if (param_1[0xf] != 0) {
      FUN_0041f670((LPVOID)param_1[0xf]);
      param_1[0xf] = 0;
    }
    FUN_004472c0((void *)param_1[0x10]);
    param_1[0x10] = 0;
  }
  return;
}

