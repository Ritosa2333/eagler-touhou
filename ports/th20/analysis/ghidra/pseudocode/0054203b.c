/* Entry: 0x0054203b; symbol: FUN_0054203b; body bytes: 110 */

UINT * __cdecl FUN_0054203b(UINT *param_1)

{
  wchar_t *pwVar1;
  UINT UVar2;
  ushort *puVar3;
  wchar_t **ppwVar4;
  short *psVar5;
  int iVar6;
  undefined4 *puVar7;
  
  UVar2 = ____lc_codepage_func();
  *param_1 = UVar2;
  UVar2 = FUN_00551400(0x100,2);
  param_1[1] = UVar2;
  if (UVar2 == 0) {
    puVar3 = ___pctype_func();
    param_1[1] = (UINT)puVar3;
    param_1[2] = 0;
  }
  else {
    puVar3 = ___pctype_func();
    param_1[2] = 1;
    puVar7 = (undefined4 *)param_1[1];
    for (iVar6 = 0x80; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar7 = *(undefined4 *)puVar3;
      puVar3 = puVar3 + 2;
      puVar7 = puVar7 + 1;
    }
  }
  ppwVar4 = ____lc_locale_name_func();
  pwVar1 = ppwVar4[1];
  param_1[3] = (UINT)pwVar1;
  if (pwVar1 != (wchar_t *)0x0) {
    psVar5 = FUN_005522f0(pwVar1);
    param_1[3] = (UINT)psVar5;
  }
  return param_1;
}

