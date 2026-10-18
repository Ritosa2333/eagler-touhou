/* Entry: 0x0052ed00; symbol: FUN_0052ed00; body bytes: 231 */

void __fastcall
FUN_0052ed00(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  char **ppcVar1;
  int iVar2;
  int *piVar3;
  void *this;
  undefined4 *puVar4;
  char **in_stack_ffffffd8;
  int local_14;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = FUN_0052eb80((int)param_1);
  while (0 < local_14) {
    local_14 = local_14 + -1;
    piVar3 = (int *)FUN_0052eb60((int)param_1);
    ppcVar1 = (char **)(*piVar3 + local_14 * 4);
    if (*ppcVar1 != (char *)0x0) {
      this = (void *)FUN_0040f160(param_1);
      FUN_00497fc0(this,*ppcVar1,4);
    }
    std::_Adl_verify_range<char*,char_const*>(ppcVar1,in_stack_ffffffd8);
  }
  FUN_0040c080(local_c,4);
  puVar4 = (undefined4 *)FUN_0040f160(param_1);
  FUN_0040b670(local_c,puVar4);
  piVar3 = (int *)FUN_0052eba0((int)param_1);
  iVar2 = *piVar3;
  puVar4 = (undefined4 *)FUN_0052eb60((int)param_1);
  FUN_00497fc0(local_c,*puVar4,iVar2);
  puVar4 = (undefined4 *)FUN_0052eb60((int)param_1);
  *puVar4 = 0;
  puVar4 = (undefined4 *)FUN_0052eba0((int)param_1);
  *puVar4 = 0;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

