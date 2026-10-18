/* Entry: 0x0054bea0; symbol: FUN_0054bea0; body bytes: 92 */

ulonglong __cdecl FUN_0054bea0(undefined4 *param_1)

{
  ulonglong uVar1;
  int in_stack_00000018;
  int in_stack_0000001c;
  undefined1 *in_stack_00000020;
  undefined4 in_stack_00000028;
  undefined1 auStack_64 [32];
  undefined4 uStack_44;
  int local_30 [11];
  
  uStack_44 = 0x54beb6;
  FUN_00547db0(local_30,param_1);
  uStack_44 = in_stack_00000028;
  FUN_0054c39a(auStack_64,(undefined4 *)&stack0x00000008);
  uVar1 = FUN_0054b871(local_30);
  FUN_00547df8(local_30);
  if ((in_stack_00000020 != (undefined1 *)0x0) && (in_stack_00000018 == 0 && in_stack_0000001c == 0)
     ) {
    *in_stack_00000020 = 0;
  }
  return uVar1;
}

