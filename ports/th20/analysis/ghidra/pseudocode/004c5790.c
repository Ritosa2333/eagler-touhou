/* Entry: 0x004c5790; symbol: FUN_004c5790; body bytes: 201 */

undefined4 __fastcall FUN_004c5790(undefined4 *param_1)

{
  bool bVar1;
  
  switch(param_1[0x3f]) {
  case 0:
    FUN_004c61d0(param_1,1);
    break;
  case 1:
    FUN_004c7160(param_1);
    break;
  case 2:
    FUN_004c6230(param_1);
    break;
  case 3:
    bVar1 = FUN_004235c0(param_1 + 0x30,10);
    if (bVar1) {
      FUN_004217c0(param_1);
      return 1;
    }
  }
  FUN_00423540(param_1 + 0x30);
  bVar1 = FUN_00461070(param_1 + 0x3b,0);
  if (bVar1) {
    FUN_00429420(param_1 + 0x3b);
  }
  return 1;
}

