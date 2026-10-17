/* Entry: 0x00454340; symbol: FUN_00454340; body bytes: 143 */

void * __thiscall FUN_00454340(void *this,uint *param_1,undefined1 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  byte local_6;
  undefined1 local_5;
  
  iVar1 = *(int *)((int)this + 0x14);
  if ((undefined1 *)(*(int *)((int)this + 0x18) - iVar1) < param_2) {
    this = (void *)FUN_00452c20(this,(uint)param_2,(uint)local_6,param_1,(uint)param_2);
  }
  else {
    *(undefined1 **)((int)this + 0x14) = param_2 + iVar1;
    puVar2 = Myptr((undefined4 *)((int)this + 4));
    FUN_00411480((uint *)((int)puVar2 + iVar1),param_1,param_2);
    local_5 = 0;
    FUN_0040fe90(param_2 + iVar1 + (int)puVar2,&local_5);
  }
  return this;
}

