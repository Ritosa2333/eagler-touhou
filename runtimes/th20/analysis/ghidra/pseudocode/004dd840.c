/* Entry: 0x004dd840; symbol: FUN_004dd840; body bytes: 136 */

void __fastcall FUN_004dd840(undefined4 param_1)

{
  if (DAT_005c4edc != (int *)0x0) {
    (**(code **)(*DAT_005c4edc + 8))(DAT_005c4edc,param_1);
    DAT_005c4edc = (int *)0x0;
  }
  if (DAT_005c4ee0 != (int *)0x0) {
    (**(code **)(*DAT_005c4ee0 + 8))(DAT_005c4ee0);
    DAT_005c4ee0 = (int *)0x0;
  }
  if (DAT_005c4ee4 != (int *)0x0) {
    (**(code **)(*DAT_005c4ee4 + 8))(DAT_005c4ee4);
    DAT_005c4ee4 = (int *)0x0;
  }
  DAT_005c4edc = (int *)0x0;
  return;
}

