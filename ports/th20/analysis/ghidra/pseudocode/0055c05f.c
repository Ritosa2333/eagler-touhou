/* Entry: 0x0055c05f; symbol: FUN_0055c05f; body bytes: 185 */

uint __cdecl FUN_0055c05f(uint *param_1,undefined1 *param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  undefined8 uVar10;
  
  uVar9 = *param_1;
  uVar1 = param_1[1];
  uVar10 = __alldiv(uVar9,uVar1,0x1e13380,0);
  iVar6 = (int)uVar10;
  uVar5 = iVar6 + 0x46;
  iVar2 = __crt_time_elapsed_leap_years<>(uVar5);
  uVar3 = (uint)((longlong)iVar2 * 0x15180);
  uVar4 = (uint)((longlong)iVar6 * 0x1e13380);
  uVar7 = uVar9 - uVar4;
  uVar8 = uVar7 - uVar3;
  uVar9 = (((uVar1 - (int)((ulonglong)((longlong)iVar6 * 0x1e13380) >> 0x20)) -
           (uint)(uVar9 < uVar4)) - (int)((ulonglong)((longlong)iVar2 * 0x15180) >> 0x20)) -
          (uint)(uVar7 < uVar3);
  *param_1 = uVar8;
  param_1[1] = uVar9;
  if (((int)uVar9 < 1) && ((int)uVar9 < 0)) {
    *param_1 = uVar8 + 0x1e13380;
    uVar9 = uVar9 + (0xfe1ecc7f < uVar8);
    uVar5 = iVar6 + 0x45;
    param_1[1] = uVar9;
    iVar6 = FUN_00552818(uVar5);
    if ((char)iVar6 != '\0') {
      *param_1 = uVar8 + 0x1e28500;
      param_1[1] = uVar9 + (0xfffeae7f < uVar8 + 0x1e13380);
      *param_2 = 1;
    }
  }
  else {
    iVar6 = FUN_00552818(uVar5);
    if ((char)iVar6 != '\0') {
      *param_2 = 1;
    }
  }
  return uVar5;
}

