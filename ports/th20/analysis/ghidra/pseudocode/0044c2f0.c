/* Entry: 0x0044c2f0; symbol: FUN_0044c2f0; body bytes: 308 */

undefined4 __thiscall FUN_0044c2f0(void *this,void *param_1)

{
  int iVar1;
  
  if (*(int *)((int)param_1 + 0x568) != 0) {
    (**(code **)(**(int **)((int)param_1 + 0x568) + 0xc))();
    FUN_0041f7c0(*(undefined4 **)((int)param_1 + 0x568));
    *(undefined4 *)((int)param_1 + 0x568) = 0;
  }
  FUN_0044c5f0((int)param_1);
  FUN_00411ce0((int)param_1 + 0x4ec);
  FUN_00411ce0((int)param_1 + 0x53c);
  *(undefined4 *)((int)param_1 + 0x558) = 0;
  *(undefined4 *)((int)param_1 + 0x55c) = 0;
  iVar1 = FUN_0044d970(this,(uint)param_1);
  if (iVar1 == 0) {
    FUN_00447410(param_1);
  }
  else {
    iVar1 = FUN_004494d0((void *)((int)this + 0x710),*(int *)((int)param_1 + 0x4c4));
    *(undefined1 *)(iVar1 + 0x5f8) = 0;
    iVar1 = FUN_004494d0((void *)((int)this + 0x710),*(int *)((int)param_1 + 0x4c4));
    FUN_00411ee0((void *)((int)this + 0x6000718),iVar1 + 0x5e4);
    FUN_0044c240((int)param_1);
    FUN_004117a0((void *)((int)param_1 + 0x4c0),0);
  }
  return 0;
}

