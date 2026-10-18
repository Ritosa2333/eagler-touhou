/* Entry: 0x004b78e0; symbol: FUN_004b78e0; body bytes: 221 */

void __fastcall
FUN_004b78e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  char **ppcVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  char **in_stack_ffffffd8;
  int local_14;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = FUN_004b6dc0(param_1);
  while (0 < local_14) {
    local_14 = local_14 + -1;
    pdVar3 = FUN_004b6da0(param_1);
    ppcVar1 = (char **)(*(int *)pdVar3 + local_14 * 4);
    if (*ppcVar1 != (char *)0x0) {
      FUN_0040f160(param_1);
      FUN_004b7c50(*ppcVar1,4);
    }
    std::_Adl_verify_range<char*,char_const*>(ppcVar1,in_stack_ffffffd8);
  }
  FUN_0040f160(param_1);
  FUN_0040b120(&local_9);
  pdVar3 = FUN_004b6de0(param_1);
  iVar2 = *(int *)pdVar3;
  pdVar3 = FUN_004b6da0(param_1);
  FUN_004b7c50(*(LPVOID *)pdVar3,iVar2);
  pdVar3 = FUN_004b6da0(param_1);
  *(undefined4 *)pdVar3 = 0;
  pdVar3 = FUN_004b6de0(param_1);
  *(undefined4 *)pdVar3 = 0;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

