/* Entry: 0x0054a779; symbol: FUN_0054a779; body bytes: 138 */

void __cdecl FUN_0054a779(int *param_1)

{
  __acrt_ptd *p_Var1;
  uint uVar2;
  int in_stack_00000018;
  int in_stack_0000001c;
  undefined1 *in_stack_00000020;
  uint *in_stack_00000028;
  uint local_314 [195];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((in_stack_00000028 == (uint *)0x0) || (param_1 == (int *)0x0)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    if ((in_stack_00000020 != (undefined1 *)0x0) &&
       (in_stack_00000018 == 0 && in_stack_0000001c == 0)) {
      *in_stack_00000020 = 0;
    }
  }
  else {
    uVar2 = FUN_0054a88d(param_1,(input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
                                  *)&stack0x00000008,(int *)local_314);
    FUN_0054b605(uVar2,local_314,in_stack_00000028);
    if ((in_stack_00000020 != (undefined1 *)0x0) &&
       (in_stack_00000018 == 0 && in_stack_0000001c == 0)) {
      *in_stack_00000020 = 0;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

