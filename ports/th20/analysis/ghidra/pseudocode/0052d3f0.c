/* Entry: 0x0052d3f0; symbol: FUN_0052d3f0; body bytes: 95 */

undefined4 * __cdecl FUN_0052d3f0(undefined4 *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  while( true ) {
    bVar1 = FUN_0052dca0(&stack0x00000008,(int)&stack0x00000010);
    if (bVar1) break;
    puVar2 = (undefined4 *)FUN_0052ddd0((int)&stack0x00000010);
    uVar3 = FUN_0052dd30(puVar2);
    puVar2 = (undefined4 *)move<>(uVar3);
    uVar3 = *puVar2;
    puVar2 = (undefined4 *)FUN_0052ddd0((int)&stack0x00000018);
    puVar2 = (undefined4 *)FUN_0052dd30(puVar2);
    *puVar2 = uVar3;
  }
  *param_1 = in_stack_00000018;
  param_1[1] = in_stack_0000001c;
  return param_1;
}

