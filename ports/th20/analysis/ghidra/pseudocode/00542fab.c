/* Entry: 0x00542fab; symbol: __ArrayUnwind; body bytes: 76 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    void __stdcall __ArrayUnwind(void *,unsigned int,unsigned int,void (__thiscall*)(void *))
   
   Library: Visual Studio 2019 Release */

void __ArrayUnwind(void *param_1,uint param_2,uint param_3,_func_void_void_ptr *param_4)

{
  uint uVar1;
  undefined4 *unaff_FS_OFFSET;
  void *pvVar2;
  undefined4 local_14;
  
  for (uVar1 = 0; uVar1 != param_3; uVar1 = uVar1 + 1) {
    pvVar2 = (void *)0x542fd6;
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*param_4)(pvVar2);
  }
  *unaff_FS_OFFSET = local_14;
  return;
}

