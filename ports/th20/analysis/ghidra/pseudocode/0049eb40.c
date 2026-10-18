/* Entry: 0x0049eb40; symbol: FUN_0049eb40; body bytes: 78 */

undefined4 * __thiscall
FUN_0049eb40(void *this,undefined4 param_1,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_2,undefined4 *param_3,undefined4 *param_4,undefined4 param_5)

{
  *(undefined4 *)this = param_1;
  FUN_0046a780(param_2);
  *(undefined4 *)((int)this + 0x20) = *param_3;
  *(undefined4 *)((int)this + 0x24) = *param_4;
  *(undefined4 *)((int)this + 0x28) = param_5;
  return (undefined4 *)this;
}

