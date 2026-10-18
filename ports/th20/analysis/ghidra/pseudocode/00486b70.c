/* Entry: 0x00486b70; symbol: FUN_00486b70; body bytes: 56 */

undefined4 * __thiscall
FUN_00486b70(void *this,undefined4 param_1,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_2,undefined4 param_3)

{
  *(undefined4 *)this = param_1;
  FUN_0046a780(param_2);
  *(undefined4 *)((int)this + 0x20) = param_3;
  return (undefined4 *)this;
}

