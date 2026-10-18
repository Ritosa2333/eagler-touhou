/* Entry: 0x0049da70; symbol: FUN_0049da70; body bytes: 250 */

undefined4 __fastcall FUN_0049da70(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_0044ee50(DAT_005c0028,7,(char **)"bullet.anm");
  piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x10),1);
  *piVar2 = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar3 = 0xffffffff;
  }
  else {
    iVar1 = FUN_0044ee50(DAT_005c0028,8,(char **)"effect.anm");
    piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x10),0);
    *piVar2 = iVar1;
    if (iVar1 == 0) {
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
      uVar3 = 0xffffffff;
    }
    else {
      iVar1 = FUN_0044ee50(DAT_005c0028,0x15,(char **)"screenswitch.anm");
      piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x10),2);
      *piVar2 = iVar1;
      if (iVar1 == 0) {
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
        uVar3 = 0xffffffff;
      }
      else {
        *(undefined4 *)(param_1 + 0x38) = 1;
        uVar3 = 0;
      }
    }
  }
  return uVar3;
}

