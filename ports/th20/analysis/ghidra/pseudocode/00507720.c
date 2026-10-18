/* Entry: 0x00507720; symbol: FUN_00507720; body bytes: 169 */

void * __fastcall FUN_00507720(void *param_1)

{
  int iVar1;
  
  FUN_005072d0(param_1);
  _memset((void *)((int)param_1 + 0x34bc0),0,36000);
  *(undefined4 *)((int)param_1 + 0x3d860) = 0;
  *(undefined4 *)((int)param_1 + 0x3d864) = 0;
  FUN_00418a10((undefined4 *)((int)param_1 + 0x3d868));
  iVar1 = FUN_00507ad0(param_1,0);
  *(int *)((int)param_1 + 0x3d860) = iVar1;
  iVar1 = FUN_00411c10((void *)((int)param_1 + 0x34bc0),0);
  *(int *)((int)param_1 + 0x3d864) = iVar1;
  FUN_0044a670((void *)((int)param_1 + 0x3d868),param_1);
  return param_1;
}

