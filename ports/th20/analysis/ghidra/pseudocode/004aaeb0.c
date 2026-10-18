/* Entry: 0x004aaeb0; symbol: FUN_004aaeb0; body bytes: 145 */

int __thiscall FUN_004aaeb0(void *this,undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 *puVar2;
  undefined1 local_24 [8];
  void *local_1c;
  default_delete<class_std::_Facet_base> *local_18;
  void *local_14;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_10;
  default_delete<class_std::_Facet_base> *local_c;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_10 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)((int)this + 0x6000730);
  local_14 = this;
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first(local_10);
  local_18 = (default_delete<class_std::_Facet_base> *)FUN_0041be70((int)local_10);
  do {
    if (local_8 == local_18) {
      return 0;
    }
    local_c = local_8;
    if (*(int *)local_8 != 0) {
      local_1c = (void *)(*(int *)local_8 + 4);
      puVar2 = (undefined4 *)FUN_004a3e60(local_1c,local_24);
      cVar1 = FUN_004a1580(param_1,param_2,(uint *)*puVar2,puVar2[1]);
      if (cVar1 != '\0') {
        return *(int *)local_c;
      }
    }
    local_8 = local_8 + 4;
  } while( true );
}

