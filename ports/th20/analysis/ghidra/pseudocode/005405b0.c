/* Entry: 0x005405b0; symbol: FUN_005405b0; body bytes: 170 */

undefined4 __thiscall FUN_005405b0(void *this,int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  char **in_stack_fffffff0;
  
  if (*(int *)((int)this + 0x10) + param_1 < 0x100) {
    uVar1 = *(undefined4 *)((int)this + 0x10);
    *(int *)((int)this + 0x10) = *(int *)((int)this + 0x10) + param_1;
    uVar2 = FUN_0053f750((int)this);
    iVar3 = *(int *)((int)this + 0x10) + 4;
    if (uVar2 < (uint)((int)(iVar3 + (iVar3 >> 0x1f & 3U)) >> 2)) {
      iVar3 = *(int *)((int)this + 0x10) + 4;
      FUN_0053f370(this,(int)(iVar3 + (iVar3 >> 0x1f & 3U)) >> 2);
    }
    FUN_0053f260(this,4,(uint *)((int)this + 0x14),'\0');
    *(undefined4 *)((int)this + 0x14) = uVar1;
    uVar1 = 0;
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)"Script stack over\n",in_stack_fffffff0);
    uVar1 = 0xffffffff;
  }
  return uVar1;
}

