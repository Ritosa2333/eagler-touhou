/* Entry: 0x004f4ef0; symbol: _Locinfo; body bytes: 215 */

/* Library Function - Single Match
    public: __thiscall std::_Locinfo::_Locinfo(char const *)
   
   Library: Visual Studio 2019 Release */

_Locinfo * __thiscall std::_Locinfo::_Locinfo(_Locinfo *this,char *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a47f;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Lockit::_Lockit((_Lockit *)this,0);
  local_8 = 0;
  _Yarn<char>::_Yarn<char>((_Yarn<char> *)(this + 4));
  local_8._0_1_ = 1;
  _Yarn<char>::_Yarn<char>((_Yarn<char> *)(this + 0xc));
  local_8._0_1_ = 2;
  _Yarn<wchar_t>::_Yarn<wchar_t>((_Yarn<wchar_t> *)(this + 0x14));
  local_8._0_1_ = 3;
  _Yarn<wchar_t>::_Yarn<wchar_t>((_Yarn<wchar_t> *)(this + 0x1c));
  local_8._0_1_ = 4;
  _Yarn<char>::_Yarn<char>((_Yarn<char> *)(this + 0x24));
  local_8._0_1_ = 5;
  _Yarn<char>::_Yarn<char>((_Yarn<char> *)(this + 0x2c));
  local_8 = CONCAT31(local_8._1_3_,6);
  if (param_1 == (char *)0x0) {
    _Xruntime_error("bad locale name");
  }
  else {
    FUN_00541c22((int)this,(uint *)param_1);
  }
  *unaff_FS_OFFSET = local_10;
  return this;
}

