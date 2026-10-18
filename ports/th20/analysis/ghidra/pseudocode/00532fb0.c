/* Entry: 0x00532fb0; symbol: FUN_00532fb0; body bytes: 237 */

undefined4 __thiscall FUN_00532fb0(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  char **in_stack_fffffff0;
  
  std::_Adl_verify_range<char*,char_const*>
            ((char **)"initialize WeaponStoneInf\n",in_stack_fffffff0);
  FUN_005345d0(this,param_1);
  FUN_005347b0(*(void **)((int)this + 0x80),this);
  FUN_004776a0(DAT_005c0028,param_1);
  iVar1 = FUN_0044ee50(DAT_005c0028,0x17,(char **)"aura.anm");
  *(int *)((int)this + 0x24) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar2 = 0xffffffff;
  }
  else {
    piVar3 = FUN_004122c0(0x1b,FUN_00534070,this);
    *(int **)((int)this + 8) = piVar3;
    piVar3 = FUN_00412360(0x37,FUN_0049dea0,this);
    *(int **)((int)this + 0xc) = piVar3;
    FUN_00534080(this,1);
    FUN_00534080(this,0);
    FUN_00423520((void *)((int)this + 0x10),0);
    FUN_00423520((void *)((int)this + 100),0);
    uVar2 = 0;
  }
  return uVar2;
}

