/* Entry: 0x00486af0; symbol: FUN_00486af0; body bytes: 64 */

undefined4 * __thiscall FUN_00486af0(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  FUN_0046a710((void *)((int)this + 4),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 1));
  *(undefined4 *)((int)this + 0x20) = param_1[8];
  return (undefined4 *)this;
}

