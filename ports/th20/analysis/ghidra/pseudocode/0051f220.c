/* Entry: 0x0051f220; symbol: FUN_0051f220; body bytes: 175 */

undefined4 __fastcall FUN_0051f220(int param_1)

{
  if (DAT_005c49e8 == 0) {
    switch(*(undefined4 *)(param_1 + 0x18)) {
    case 1:
      FUN_0052a700(param_1);
      break;
    case 8:
      FUN_00529b40(param_1);
      break;
    case 0xb:
      FUN_005257f0(param_1);
      break;
    case 0xc:
      FUN_005240d0(param_1);
      break;
    case 0xe:
      FUN_00520c80(param_1);
      break;
    case 0xf:
      FUN_00522e20(param_1);
      break;
    case 0x10:
      FUN_005277f0(param_1);
      break;
    case 0x13:
    case 0x14:
      FUN_00528a50(param_1);
      break;
    case 0x17:
      FUN_0052b480(param_1);
    }
  }
  return 1;
}

