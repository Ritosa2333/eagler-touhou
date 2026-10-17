/* Entry: 0x0052d450; symbol: FUN_0052d450; body bytes: 100 */

undefined4 * __cdecl FUN_0052d450(undefined4 *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  while( true ) {
    bVar1 = FUN_0052dca0(&stack0x00000008,(int)&stack0x00000010);
    if (bVar1) break;
    uVar2 = FUN_0052dd30((undefined4 *)&stack0x00000008);
    puVar3 = (undefined4 *)move<>(uVar2);
    uVar2 = *puVar3;
    puVar3 = (undefined4 *)FUN_0052dd30(&stack0x00000018);
    *puVar3 = uVar2;
    FUN_0052dd90((int)&stack0x00000018);
    FUN_0052dd90((int)&stack0x00000008);
  }
  *param_1 = in_stack_00000018;
  param_1[1] = in_stack_0000001c;
  return param_1;
}

