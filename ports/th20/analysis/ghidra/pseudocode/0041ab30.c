/* Entry: 0x0041ab30; symbol: FUN_0041ab30; body bytes: 169 */

void FUN_0041ab30(void)

{
  HWND pHVar1;
  UINT UVar2;
  int iVar3;
  int local_8;
  
  iVar3 = 0xca;
  pHVar1 = (HWND)FUN_0040ff90(0x5b6758);
  UVar2 = IsDlgButtonChecked(pHVar1,iVar3);
  if (UVar2 == 1) {
    DAT_005c4f8c = DAT_005c4f8c | 0x80;
  }
  else {
    DAT_005c4f8c = DAT_005c4f8c & 0xffffff7f;
  }
  local_8 = 0;
  while( true ) {
    if (9 < local_8) {
      return;
    }
    iVar3 = *(int *)(&DAT_005ae128 + local_8 * 4);
    pHVar1 = (HWND)FUN_0040ff90(0x5b6758);
    UVar2 = IsDlgButtonChecked(pHVar1,iVar3);
    if (UVar2 == 1) break;
    local_8 = local_8 + 1;
  }
  DAT_005c4f80 = local_8;
  DAT_005c4f8a = (&DAT_005ae1d0)[local_8 * 4];
  return;
}

