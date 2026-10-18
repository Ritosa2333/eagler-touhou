/* Entry: 0x0049ea80; symbol: FUN_0049ea80; body bytes: 88 */

undefined4 * __thiscall FUN_0049ea80(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  FUN_0046a710((void *)((int)this + 4),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 1));
  *(undefined4 *)((int)this + 0x20) = param_1[8];
  *(undefined4 *)((int)this + 0x24) = param_1[9];
  *(undefined4 *)((int)this + 0x28) = param_1[10];
  return (undefined4 *)this;
}

