/* Entry: 0x0047d960; symbol: FUN_0047d960; body bytes: 211 */

undefined4 __fastcall FUN_0047d960(void *param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (((*(uint *)((int)param_1 + 0x14) >> 10 & 1) == 0) &&
     (iVar2 = FUN_00485b60(param_1), iVar2 != 0)) {
    uVar3 = 0xffffffff;
  }
  else {
    bVar1 = FUN_0047c650((void *)((int)param_1 + 0x5c),0);
    if (bVar1) {
      FUN_004502c0((void *)((int)param_1 + 0x5c),(undefined4 *)((int)param_1 + 100));
    }
    if (((*(int *)((int)param_1 + 0x50) == 1) ||
        ((*(int *)((int)param_1 + 0x50) == 2 &&
         ((bVar1 = FUN_004235c0((void *)((int)param_1 + 0x4f8),8), !bVar1 ||
          (iVar2 = FUN_00484a20(param_1,0), iVar2 != 1)))))) &&
       ((*(uint *)((int)param_1 + 0x14) >> 9 & 1) == 0)) {
      FUN_00484a20(param_1,0);
    }
    *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffbff;
    uVar3 = 0;
  }
  return uVar3;
}

