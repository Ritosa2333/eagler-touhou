/* Entry: 0x004e20d0; symbol: FUN_004e20d0; body bytes: 379 */

undefined4 __fastcall FUN_004e20d0(void *param_1)

{
  uint uVar1;
  int iVar2;
  uint local_14;
  uint local_10;
  
  switch(*(undefined4 *)((int)param_1 + 0xd0)) {
  case 0:
    uVar1 = FUN_004b8390(0x5ba568);
    if (((uVar1 == 0) && ((*(uint *)(DAT_005ba828 + 0xe8) >> 0x10 & 1) == 0)) && (DAT_005afcfc < 0))
    {
      if (DAT_005b889c == (void *)0x0) {
        local_10 = 0;
      }
      else {
        local_10 = FUN_00419c00(DAT_005b889c,0x100);
      }
      if (((local_10 != 0) || ((DAT_005c5888 >> 3 & 1) != 0)) &&
         ((iVar2 = FUN_00464350(DAT_005ba828), iVar2 != 0 &&
          (iVar2 = FUN_004e66b0(DAT_005ba828), 0x1d < iVar2)))) {
        FUN_004e5d60();
      }
    }
    if (-1 < DAT_005afcfc) {
      if (DAT_005b889c == (void *)0x0) {
        local_14 = 0;
      }
      else {
        local_14 = FUN_00419c00(DAT_005b889c,0x100);
      }
      if (local_14 != 0) {
        FUN_004a0fb0(&DAT_005c4d40,4);
        DAT_005afcfc = 0xffffffff;
        return 1;
      }
    }
    break;
  case 1:
  case 2:
  case 3:
    FUN_004e2260(param_1);
  }
  FUN_00423540((undefined4 *)((int)param_1 + 0x10));
  FUN_00423540((undefined4 *)((int)param_1 + 0x20));
  return 1;
}

