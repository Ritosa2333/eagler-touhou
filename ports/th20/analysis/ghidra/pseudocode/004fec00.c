/* Entry: 0x004fec00; symbol: FUN_004fec00; body bytes: 145 */

void * __thiscall FUN_004fec00(void *this,uint param_1,char param_2)

{
  int iVar1;
  undefined4 *puVar2;
  byte local_6;
  undefined1 local_5;
  
  iVar1 = *(int *)((int)this + 0x14);
  if ((uint)(*(int *)((int)this + 0x18) - iVar1) < param_1) {
    this = (void *)FUN_004ee600(this,param_1,(uint)local_6,param_1,param_2);
  }
  else {
    *(uint *)((int)this + 0x14) = iVar1 + param_1;
    puVar2 = Myptr((undefined4 *)((int)this + 4));
    FUN_0040fe50((undefined1 *)((int)puVar2 + iVar1),param_1,param_2);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)(iVar1 + param_1 + (int)puVar2),&local_5);
  }
  return this;
}

