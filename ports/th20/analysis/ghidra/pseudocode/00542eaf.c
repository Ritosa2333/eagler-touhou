/* Entry: 0x00542eaf; symbol: `eh_vector_constructor_iterator'; body bytes: 90 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    void __stdcall `eh vector constructor iterator'(void *,unsigned int,unsigned int,void
   (__thiscall*)(void *),void (__thiscall*)(void *))
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void _eh_vector_constructor_iterator_
               (void *param_1,uint param_2,uint param_3,_func_void_void_ptr *param_4,
               _func_void_void_ptr *param_5)

{
  uint uVar1;
  undefined4 *unaff_FS_OFFSET;
  void *pvVar2;
  undefined4 local_14;
  
  for (uVar1 = 0; uVar1 != param_3; uVar1 = uVar1 + 1) {
    pvVar2 = (void *)0x542ed4;
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*param_4)(pvVar2);
  }
  FUN_00542f0f();
  *unaff_FS_OFFSET = local_14;
  return;
}

