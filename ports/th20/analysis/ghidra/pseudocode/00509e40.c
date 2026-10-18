/* Entry: 0x00509e40; symbol: FUN_00509e40; body bytes: 149 */

bool __thiscall FUN_00509e40(void *this,undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  int iVar1;
  
  **(undefined2 **)((int)this + 0x3d860) = param_1;
  *(undefined2 *)(*(int *)((int)this + 0x3d860) + 2) = param_2;
  *(undefined2 *)(*(int *)((int)this + 0x3d860) + 4) = param_3;
  *(int *)((int)this + 0x3d860) = *(int *)((int)this + 0x3d860) + 6;
  iVar1 = FUN_00507ad0(this,0);
  return 35999 < (*(int *)((int)this + 0x3d860) - iVar1) / 6;
}

