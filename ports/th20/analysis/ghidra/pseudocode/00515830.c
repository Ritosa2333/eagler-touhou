/* Entry: 0x00515830; symbol: FUN_00515830; body bytes: 205 */

int * __thiscall
FUN_00515830(void *this,basic_streambuf<char,struct_std::char_traits<char>_> *param_1,bool param_2,
            int param_3)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056abe1;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_3 != 0) {
    *(undefined **)this = &DAT_00574d94;
    FUN_005157f0((undefined4 *)((int)this + 0x18));
    local_8 = 0;
  }
                    /* WARNING: Load size is inaccurate */
  *(undefined ***)((int)this + *(int *)(*this + 4)) = &PTR_LAB_00574d90;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
  *(int *)((int)this + *(int *)(*this + 4) + -4) = *(int *)(*this + 4) + -0x18;
  *(undefined4 *)((int)this + 8) = 0;
  *(undefined4 *)((int)this + 0xc) = 0;
                    /* WARNING: Load size is inaccurate */
  std::basic_ios<char,struct_std::char_traits<char>_>::init
            ((basic_ios<char,struct_std::char_traits<char>_> *)((int)this + *(int *)(*this + 4)),
             param_1,param_2);
  *unaff_FS_OFFSET = local_10;
  return (int *)this;
}

