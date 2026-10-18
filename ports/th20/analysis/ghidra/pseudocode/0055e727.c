/* Entry: 0x0055e727; symbol: expand_if_necessary; body bytes: 135 */

/* Library Function - Single Match
    private: int __thiscall `anonymous namespace'::argument_list<char>::expand_if_necessary(void)
   
   Library: Visual Studio 2019 Release */

int __thiscall _anon_5F5C8891::argument_list<char>::expand_if_necessary(argument_list<char> *this)

{
  int iVar1;
  LPVOID pvVar2;
  uint uVar3;
  
  if (*(int *)(this + 4) == *(int *)(this + 8)) {
    if (*(int *)this == 0) {
      pvVar2 = FUN_00557b40(4,4);
      *(LPVOID *)this = pvVar2;
      FUN_00557ba0((LPVOID)0x0);
      iVar1 = *(int *)this;
      if (iVar1 != 0) {
        *(int *)(this + 4) = iVar1;
        *(int *)(this + 8) = iVar1 + 0x10;
        goto LAB_0055e735;
      }
    }
    else {
      uVar3 = *(int *)(this + 8) - *(int *)this >> 2;
      if (uVar3 < 0x80000000) {
        pvVar2 = __recalloc_base(*(LPCVOID *)this,uVar3 * 2,4);
        if (pvVar2 == (LPVOID)0x0) {
          iVar1 = 0xc;
        }
        else {
          *(LPVOID *)this = pvVar2;
          *(LPVOID *)(this + 4) = (LPVOID)((int)pvVar2 + uVar3 * 4);
          *(LPVOID *)(this + 8) = (LPVOID)((int)pvVar2 + uVar3 * 8);
          iVar1 = 0;
        }
        FUN_00557ba0((LPVOID)0x0);
        return iVar1;
      }
    }
    iVar1 = 0xc;
  }
  else {
LAB_0055e735:
    iVar1 = 0;
  }
  return iVar1;
}

