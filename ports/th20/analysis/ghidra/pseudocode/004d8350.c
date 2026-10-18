/* Entry: 0x004d8350; symbol: FUN_004d8350; body bytes: 509 */

undefined4 FUN_004d8350(void)

{
  char cVar1;
  char **ppcVar2;
  undefined4 *puVar3;
  int iVar4;
  HWND pHVar5;
  uint uVar6;
  int iStack00000004;
  char **extraout_var;
  
  Sleep(2000);
  std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 1\n",extraout_var);
  iStack00000004 = DAT_005c4d2c;
  ppcVar2 = (char **)FUN_0044eb30(DAT_005c0028,1,(char **)"sig.anm");
  *(char ***)(iStack00000004 + 0x60c) = ppcVar2;
  if (ppcVar2 != (char **)0x0) {
    FUN_00412d80(*(int *)(iStack00000004 + 0xc));
    *(undefined4 *)(iStack00000004 + 0x610) = 1;
    std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 2\n",ppcVar2);
    DAT_005c5880 = FUN_0044eb30(DAT_005c0028,0,(char **)"text.anm");
    if (DAT_005c5880 != 0) {
      std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 3\n",ppcVar2);
      puVar3 = FUN_00471060();
      if (puVar3 != (undefined4 *)0x0) {
        *(undefined4 *)(iStack00000004 + 0x614) = 1;
        std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 3\n",ppcVar2);
        iVar4 = FUN_00426840(&DAT_005ba830,(undefined1 (*) [16])"../../bgm/thbgm.fmt");
        if (iVar4 != 0) {
          FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0057183c);
        }
        std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 5\n",ppcVar2);
        pHVar5 = (HWND)FUN_0040c300(&DAT_005b6758);
        FUN_004263e0(&DAT_005ba830,pHVar5);
        FUN_00428560((int *)&DAT_005ba830);
        std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 6\n",ppcVar2);
        cVar1 = FUN_004107a0();
        if (cVar1 != '\0') {
          uVar6 = FUN_00428fa0(0x5c4d40);
          if (uVar6 == 0) {
            FUN_004286b0(&DAT_005ba830,(char **)"thbgm.dat");
          }
          else {
            FUN_00548610(&DAT_005bfef4,0x20,0x571884);
          }
        }
        std::_Adl_verify_range<char*,char_const*>((char **)"LoadThread 7\n",ppcVar2);
        FUN_004d82c0();
        while (cVar1 = FUN_0044d880((int)DAT_005c0028), cVar1 == '\0') {
          Sleep(1);
        }
        std::_Adl_verify_range<char*,char_const*>((char **)"loading finish\n",ppcVar2);
        FUN_00412d80(*(int *)(iStack00000004 + 8));
        FUN_004515e0(0);
        return 0;
      }
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571800);
    }
  }
  FUN_004be250(&DAT_005c4d40,3);
  FUN_00412d80(*(int *)(iStack00000004 + 8));
  return 0;
}

