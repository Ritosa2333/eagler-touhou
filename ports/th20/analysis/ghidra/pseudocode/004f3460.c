/* Entry: 0x004f3460; symbol: FUN_004f3460; body bytes: 148 */

undefined4 * __cdecl FUN_004f3460(undefined4 *param_1)

{
  undefined4 *puVar1;
  void *pvVar2;
  tagCY tVar3;
  undefined4 in_stack_00000014;
  undefined4 uVar4;
  undefined1 *puVar5;
  undefined1 local_38 [16];
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  tVar3 = COleCurrency::operator_union_tagCY((COleCurrency *)&stack0x0000000c);
  local_28 = *(undefined4 *)tVar3.s.Lo;
  local_24 = ((undefined4 *)tVar3.s.Lo)[1];
  puVar5 = &stack0x00000008;
  uVar4 = 0x4f34a6;
  local_20 = local_28;
  local_1c = local_24;
  puVar1 = (undefined4 *)move<>(puVar5);
  local_18 = *puVar1;
  local_14 = local_18;
  pvVar2 = FUN_004f38d0(local_38,in_stack_00000014);
  FUN_004e6bd0(&stack0xffffffb8,pvVar2);
  FUN_004f3a30(param_1,local_18,local_28,local_24,uVar4,puVar5);
  return param_1;
}

