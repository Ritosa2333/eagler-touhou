/* Entry: 0x004b5920; symbol: FUN_004b5920; body bytes: 64 */

bool FUN_004b5920(void)

{
  int iVar1;
  
  iVar1 = FUN_0044ee50(DAT_005c0028,5,(char **)"front.anm");
  if (iVar1 != 0) {
    DAT_005c4a18 = 1;
  }
  else {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
  }
  return iVar1 == 0;
}

