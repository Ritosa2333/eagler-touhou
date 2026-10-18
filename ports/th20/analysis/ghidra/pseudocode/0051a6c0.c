/* Entry: 0x0051a6c0; symbol: FUN_0051a6c0; body bytes: 125 */

int __thiscall FUN_0051a6c0(void *this,uint param_1,uint param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  int iVar3;
  
  uVar1 = std::_String_val<struct_std::_Simple_types<char>_>::_Clamp_suffix_size
                    ((_String_val<struct_std::_Simple_types<char>_> *)((int)this + 4),param_1,
                     param_2);
  iVar3 = *(int *)((int)this + 0x14);
  puVar2 = Myptr((undefined4 *)((int)this + 4));
  iVar3 = iVar3 - uVar1;
  FUN_00411480((uint *)((int)puVar2 + param_1),(uint *)((int)((int)puVar2 + param_1) + uVar1),
               (undefined1 *)((iVar3 - param_1) + 1));
  *(int *)((int)this + 0x14) = iVar3;
  return (int)this;
}

