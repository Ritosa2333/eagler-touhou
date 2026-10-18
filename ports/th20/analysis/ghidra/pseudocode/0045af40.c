/* Entry: 0x0045af40; symbol: FUN_0045af40; body bytes: 172 */

undefined4 __thiscall FUN_0045af40(void *this,char **param_1,uint param_2)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 0x7c) == 0) {
    if (*(int *)((int)this + 0x8c) == -1) {
      FUN_0045ab40(this,*(char ***)((int)this + 0x94),(char *)param_1,(char *)0x1);
    }
    if (*(int *)((int)this + 0x8c) == -1) {
      uVar1 = 0x80004005;
    }
    else {
      *(undefined4 *)((int)this + 0x9c) = 0;
      *(char ***)((int)this + 0x90) = param_1;
      std::_Adl_verify_range<char*,char_const*>((char **)"%s %d %d\n",param_1);
      FUN_0045b5f0(this,'\0',param_2);
      *(undefined4 *)((int)this + 0x2c) = *(undefined4 *)((int)this + 8);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 0x80004005;
  }
  return uVar1;
}

