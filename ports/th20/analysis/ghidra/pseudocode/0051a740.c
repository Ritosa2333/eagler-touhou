/* Entry: 0x0051a740; symbol: _Getcat; body bytes: 232 */

/* Library Function - Single Match
    public: static unsigned int __cdecl std::ctype<char>::_Getcat(class std::locale::facet const *
   *,class std::locale const *)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

uint __cdecl std::ctype<char>::_Getcat(facet **param_1,locale *param_2)

{
  char *pcVar1;
  int *unaff_FS_OFFSET;
  _Locinfo local_60 [52];
  facet *local_2c;
  _Locinfo *local_28;
  _Locinfo *local_24;
  _Locinfo *local_20;
  facet *local_1c;
  ctype<char> *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a56d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = 0;
  if ((param_1 != (facet **)0x0) && (*param_1 == (facet *)0x0)) {
    local_18 = (ctype<char> *)operator_new(0x18);
    local_8 = 0;
    if (local_18 == (ctype<char> *)0x0) {
      local_1c = (facet *)0x0;
    }
    else {
      pcVar1 = locale::c_str(param_2);
      local_28 = (_Locinfo *)_Locinfo::_Locinfo(local_60,pcVar1);
      local_8 = CONCAT31(local_8._1_3_,1);
      local_14 = local_14 | 1;
      local_24 = local_28;
      local_20 = local_28;
      local_1c = (facet *)ctype<char>(local_18,local_28,0);
    }
    local_2c = local_1c;
    *param_1 = local_1c;
    local_8 = 0xffffffff;
    if ((local_14 & 1) != 0) {
      local_14 = local_14 & 0xfffffffe;
      _Locinfo::~_Locinfo(local_60);
    }
  }
  *unaff_FS_OFFSET = local_10;
  return 2;
}

