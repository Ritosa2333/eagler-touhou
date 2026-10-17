/* Entry: 0x0050a110; symbol: FUN_0050a110; body bytes: 90 */

int __fastcall FUN_0050a110(void *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_00411c10((void *)((int)param_1 + 0x34bc0),0);
  iVar1 = *(int *)((int)param_1 + 0x3d864);
  iVar3 = FUN_00507ad0(param_1,0);
  return ((*(int *)((int)param_1 + 0x3d860) - iVar3) / 6) * 6 + (iVar1 - iVar2);
}

