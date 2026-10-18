/* Entry: 0x00551665; symbol: ___acrt_errno_map_os_error; body bytes: 35 */

/* Library Function - Single Match
    ___acrt_errno_map_os_error
   
   Library: Visual Studio 2019 Release */

void __cdecl ___acrt_errno_map_os_error(uint param_1)

{
  __acrt_ptd *p_Var1;
  uint uVar2;
  
  p_Var1 = FUN_005516ac();
  *(uint *)p_Var1 = param_1;
  uVar2 = FUN_00551606(param_1);
  p_Var1 = FUN_005516c1();
  *(uint *)p_Var1 = uVar2;
  return;
}

