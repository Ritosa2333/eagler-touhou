/* Entry: 0x004e4d50; symbol: FUN_004e4d50; body bytes: 718 */

void __fastcall FUN_004e4d50(void *param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  float local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004e6a20(DAT_005c0698,1);
  bVar1 = FUN_0047a340((undefined4 *)((int)param_1 + 0xcc));
  if ((bVar1) && (iVar2 = FUN_0044c670((void *)((int)param_1 + 0xcc),0x5c,0), iVar2 != 0)) {
    uVar3 = FUN_004456e0(iVar2);
    FUN_0044fa10(DAT_005c4ef4,uVar3 | 0xff000000);
  }
  iVar2 = *(int *)((int)param_1 + 0xd0);
  if (iVar2 != 1) {
    if (iVar2 == 2) {
      if (*(int *)((int)param_1 + 0xd8) == 0xf) {
        FUN_004e5620(param_1);
      }
      if ((*(uint *)((int)param_1 + 0x2d0) & 3) == 1) {
        FUN_004e5240((int)param_1);
      }
      else if ((*(uint *)((int)param_1 + 0x2d0) & 3) == 2) {
        FUN_004e54c0(param_1);
      }
      else if (((*(int *)((int)param_1 + 0xd8) != 0xe) && (*(int *)((int)param_1 + 0xd8) != 0x10))
              && (bVar1 = FUN_00488850(0x5ba568), !bVar1)) {
        FUN_00422dd0(local_14,DAT_0057265c,DAT_0056d7c0,0);
        FUN_004e66d0(0x5ba568);
        FUN_0046c990(DAT_005c0698,local_14,(wchar_t *)"Credit %d");
      }
      iVar2 = FUN_00474d60(0x5ba568);
      if (((3 < iVar2) && (iVar2 = FUN_00474d60(0x5ba568), iVar2 < 7)) &&
         ((bVar1 = FUN_004bd820(0x5ba568), !bVar1 &&
          ((*(int *)((int)param_1 + 0xd8) == 6 && (iVar2 = FUN_00488830(DAT_005ba828), iVar2 == 0)))
          ))) {
        iVar2 = FUN_00464080(0);
        iVar2 = FUN_0041cab0(iVar2);
        iVar2 = FUN_004bd610(DAT_005c6108,iVar2);
        if (iVar2 < 9) {
          FUN_00422dd0(local_20,DAT_0056fa28,DAT_00572660,0);
          FUN_004e67e0(DAT_005c0698);
          FUN_00470ad0(DAT_005c0698,0xffffff80);
          FUN_004e6a20(DAT_005c0698,1);
          FUN_0040ddd0(&stack0xffffffbc,&DAT_00572604);
          FUN_0046c210(DAT_005c0698,local_20);
          FUN_004e67e0(DAT_005c0698);
        }
      }
      goto LAB_004e4ffe;
    }
    if (iVar2 != 3) goto LAB_004e4ffe;
  }
  if ((*(uint *)((int)param_1 + 0x2d0) & 3) == 1) {
    FUN_004e5240((int)param_1);
  }
  else if ((*(uint *)((int)param_1 + 0x2d0) & 3) == 2) {
    FUN_004e54c0(param_1);
  }
LAB_004e4ffe:
  FUN_004e6a20(DAT_005c0698,0);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

