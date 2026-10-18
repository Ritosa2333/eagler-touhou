/* Entry: 0x004b5960; symbol: FUN_004b5960; body bytes: 370 */

undefined4 __fastcall FUN_004b5960(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined1 (*pauVar5) [16];
  undefined8 uVar6;
  SIZE_T *pSVar7;
  int iVar8;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,6,*(char ***)(DAT_005c6110 + 0x54));
  *(int *)(param_1 + 0x170) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar2 = 0xffffffff;
  }
  else {
    if (DAT_005c4a14 == 0) {
      iVar4 = DAT_005c6110 + 0x14;
      iVar1 = FUN_004b8340(0x5ba568);
      iVar3 = FUN_004b7f70(0x5ba568);
      iVar8 = 0;
      pSVar7 = (SIZE_T *)0x0;
      pauVar5 = (undefined1 (*) [16])FUN_004bd4a0(*(int *)(iVar4 + (iVar1 + iVar3 * 8) * 4));
      uVar2 = FUN_00410aa0(pauVar5,pSVar7,iVar8);
      *(undefined4 *)(param_1 + 0x1c0) = uVar2;
      if (*(int *)(param_1 + 0x1c0) == 0) {
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
        return 0xffffffff;
      }
    }
    else {
      *(int *)(param_1 + 0x1c0) = DAT_005c4a14;
      iVar1 = DAT_005c6110 + 0x14;
      iVar3 = FUN_004b7f70(0x5ba568);
      iVar4 = FUN_004b8340(0x5ba568);
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"%s load Skip\n",*(char ***)(iVar1 + (iVar4 + iVar3 * 8) * 4));
      DAT_005c4a14 = 0;
    }
    FUN_00423520((void *)(param_1 + 0x140),0);
    uVar6 = FUN_004b8290(0x5ba568);
    *(undefined8 *)(param_1 + 0x160) = uVar6;
    *(undefined4 *)(param_1 + 0x1c4) = 0xffffffff;
    *(undefined4 *)(param_1 + 0x1cc) = 0xffffffff;
    uVar2 = 0;
  }
  return uVar2;
}

