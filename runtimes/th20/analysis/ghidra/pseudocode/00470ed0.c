/* Entry: 0x00470ed0; symbol: FUN_00470ed0; body bytes: 379 */

void __thiscall FUN_00470ed0(void *this,int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00437ca0(this,param_1);
  *(int *)((int)this + 0x20) = param_1;
  *(undefined4 *)((int)this + 0x388) = *(undefined4 *)(iVar1 + 0x38);
  *(undefined4 *)((int)this + 0x378) = *(undefined4 *)((int)this + 0x388);
  *(undefined4 *)((int)this + 0x390) = *(undefined4 *)(iVar1 + 0x40);
  *(undefined4 *)((int)this + 0x380) = *(undefined4 *)((int)this + 0x390);
  *(undefined4 *)((int)this + 900) = *(undefined4 *)(iVar1 + 0x3c);
  *(undefined4 *)((int)this + 0x37c) = *(undefined4 *)((int)this + 900);
  *(undefined4 *)((int)this + 0x394) = *(undefined4 *)(iVar1 + 0x44);
  *(undefined4 *)((int)this + 0x38c) = *(undefined4 *)((int)this + 0x394);
  *(float *)((int)this + 0x398) = *(float *)((int)this + 0x380) - *(float *)((int)this + 0x378);
  *(float *)((int)this + 0x39c) = *(float *)((int)this + 0x38c) - *(float *)((int)this + 0x37c);
  return;
}

