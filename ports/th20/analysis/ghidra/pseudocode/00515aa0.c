/* Entry: 0x00515aa0; symbol: ctype<char>; body bytes: 106 */

/* Library Function - Single Match
    public: __thiscall std::ctype<char>::ctype<char>(class std::_Locinfo const &,unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

ctype<char> * __thiscall
std::ctype<char>::ctype<char>(ctype<char> *this,_Locinfo *param_1,uint param_2)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ac0d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FID_conflict_codecvt_base(this,param_2);
  local_8 = 0;
  *(undefined ***)this = &PTR__scalar_deleting_destructor__00574cec;
  _Init(this,param_1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

