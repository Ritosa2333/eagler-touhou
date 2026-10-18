/* Entry: 0x005519d9; symbol: ____mb_cur_max_func; body bytes: 37 */

/* Library Function - Single Match
    ____mb_cur_max_func
   
   Library: Visual Studio 2019 Release */

int __cdecl ____mb_cur_max_func(void)

{
  int iVar1;
  int local_8;
  
  iVar1 = FUN_005579c2();
  local_8 = *(int *)(iVar1 + 0x4c);
  ___acrt_update_locale_info(iVar1,&local_8);
  return *(int *)(local_8 + 4);
}

