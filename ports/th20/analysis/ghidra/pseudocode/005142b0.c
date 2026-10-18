/* Entry: 0x005142b0; symbol: FUN_005142b0; body bytes: 224 */

int * __thiscall FUN_005142b0(void *this,int param_1,uint param_2,int param_3)

{
  basic_streambuf<char,struct_std::char_traits<char>_> *pbVar1;
  int *unaff_FS_OFFSET;
  bool bVar2;
  int iVar3;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056aaec;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_3 != 0) {
    *(undefined **)this = &DAT_00574e24;
    FUN_005157f0((undefined4 *)((int)this + 0x60));
    local_8 = 0;
  }
  iVar3 = 0;
  bVar2 = false;
  pbVar1 = (basic_streambuf<char,struct_std::char_traits<char>_> *)move<>((int)this + 0x10);
  FUN_00515830(this,pbVar1,bVar2,iVar3);
  local_8 = 1;
                    /* WARNING: Load size is inaccurate */
  *(undefined ***)((int)this + *(int *)(*this + 4)) = &PTR_LAB_00574e20;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
  *(int *)((int)this + *(int *)(*this + 4) + -4) = *(int *)(*this + 4) + -0x60;
  FUN_00514390((void *)((int)this + 0x10),param_1,param_2 | 1);
  *unaff_FS_OFFSET = local_10;
  return (int *)this;
}

