/* Entry: 0x004e9810; symbol: FUN_004e9810; body bytes: 98 */

undefined4 * __cdecl FUN_004e9810(undefined4 *param_1,undefined4 param_2,char *param_3)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *this;
  default_delete<class_std::_Facet_base> *this_00;
  char *pcVar1;
  undefined4 local_c;
  char *local_8;
  
  if (param_3 == (char *)0x0) {
    func_0x004fcd20("String pointer is null.");
  }
  while (*param_3 != '\0') {
    local_8 = param_3;
    pcVar1 = param_3;
    param_3 = param_3 + 1;
    this = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_004f57f0(&param_2,&local_c);
    this_00 = std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first(this);
    FUN_004f5690(this_00,pcVar1);
  }
  *param_1 = param_2;
  return param_1;
}

