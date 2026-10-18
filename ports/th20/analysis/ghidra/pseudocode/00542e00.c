/* Entry: 0x00542e00; symbol: FUN_00542e00; body bytes: 66 */

void __fastcall FUN_00542e00(uint param_1,undefined4 param_2)

{
  undefined1 auVar1 [16];
  
  if (((byte)DAT_005b2610 & 0x20) == 0) {
    auVar1 = vpinsrd_avx(ZEXT416(param_1),param_2,1);
    vcvtqq2pd_avx512vl(auVar1);
    return;
  }
  return;
}

