/* Entry: 0x0052e170; symbol: FUN_0052e170; body bytes: 157 */

undefined4 __fastcall FUN_0052e170(void *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,0x14,(char **)"trophy.anm");
  *(int *)((int)param_1 + 0x10) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571f5c);
    uVar2 = 0xffffffff;
  }
  else {
    piVar3 = FUN_004122c0(0x11,FUN_0052f030,param_1);
    *(int **)((int)param_1 + 8) = piVar3;
    piVar3 = FUN_00412360(0x5d,FUN_0049dea0,param_1);
    *(int **)((int)param_1 + 0xc) = piVar3;
    FUN_0052efb0((void *)((int)param_1 + 0x14));
    FUN_0052f800(param_1,0);
    uVar2 = 0;
  }
  return uVar2;
}

