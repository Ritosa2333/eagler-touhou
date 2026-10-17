/* Entry: 0x00547378; symbol: FUN_00547378; body bytes: 123 */

void FUN_00547378(void)

{
  int iVar1;
  int unaff_EBX;
  int unaff_EBP;
  int *unaff_ESI;
  
  *(undefined4 *)(*(int *)(unaff_EBP + 0xc) + -4) = *(undefined4 *)(unaff_EBP + -0x30);
  __FindAndUnlinkFrame(*(int *)(unaff_EBP + -0x34));
  iVar1 = ___vcrt_getptd();
  *(undefined4 *)(iVar1 + 0x10) = *(undefined4 *)(unaff_EBP + -0x38);
  iVar1 = ___vcrt_getptd();
  *(undefined4 *)(iVar1 + 0x14) = *(undefined4 *)(unaff_EBP + -0x3c);
  if ((((*unaff_ESI == -0x1f928c9d) && (unaff_ESI[4] == 3)) &&
      ((unaff_ESI[5] == 0x19930520 || ((unaff_ESI[5] == 0x19930521 || (unaff_ESI[5] == 0x19930522)))
       ))) && ((*(int *)(unaff_EBP + -0x40) == 0 && (unaff_EBX != 0)))) {
    iVar1 = __IsExceptionObjectToBeDestroyed(unaff_ESI[6]);
    if (iVar1 != 0) {
      FUN_005463e0(unaff_ESI);
    }
  }
  return;
}

