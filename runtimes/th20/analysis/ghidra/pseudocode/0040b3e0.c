/* Entry: 0x0040b3e0; symbol: FUN_0040b3e0; body bytes: 236 */

void __thiscall FUN_0040b3e0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  void *_ArgList;
  uintptr_t uVar3;
  uint *_ThrdAddr;
  int *unaff_FS_OFFSET;
  uint _InitFlag;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_18 [4];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005675cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_18,4);
  uVar1 = move<>(param_2);
  uVar2 = move<>(param_1);
  FUN_0040b550(local_18,uVar2,uVar1);
  local_8 = 0;
  _ThrdAddr = (uint *)((int)this + 4);
  _InitFlag = 0;
  _ArgList = (void *)FUN_0040c300((undefined4 *)local_18);
  uVar3 = __beginthreadex((void *)0x0,0,FUN_0040b310,_ArgList,_InitFlag,_ThrdAddr);
  *(uintptr_t *)this = uVar3;
                    /* WARNING: Load size is inaccurate */
  if (*this == 0) {
    *(undefined4 *)((int)this + 4) = 0;
    std::_Throw_Cpp_error(6);
  }
  else {
    std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
    release((unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
             *)local_18);
  }
  local_8 = 0xffffffff;
  std::unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>::
  ~unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
            ((unique_ptr<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_>
              *)local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

