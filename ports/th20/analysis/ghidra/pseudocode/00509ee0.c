/* Entry: 0x00509ee0; symbol: FUN_00509ee0; body bytes: 109 */

bool __thiscall FUN_00509ee0(void *this,undefined1 param_1)

{
  int iVar1;
  
  **(undefined1 **)((int)this + 0x3d864) = param_1;
  *(int *)((int)this + 0x3d864) = *(int *)((int)this + 0x3d864) + 1;
  iVar1 = FUN_00411c10((void *)((int)this + 0x34bc0),0);
  return 0x4af < *(int *)((int)this + 0x3d864) - iVar1;
}

