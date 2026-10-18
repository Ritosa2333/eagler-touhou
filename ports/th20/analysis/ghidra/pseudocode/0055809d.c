/* Entry: 0x0055809d; symbol: ___acrt_EnumSystemLocalesEx@16; body bytes: 84 */

/* Library Function - Single Match
    ___acrt_EnumSystemLocalesEx@16
   
   Library: Visual Studio 2019 Release */

void __thiscall
___acrt_EnumSystemLocalesEx_16
          (void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  FARPROC pFVar1;
  int local_14;
  undefined4 *local_10;
  undefined4 local_c;
  
  pFVar1 = FUN_00557cd0(this);
  if (pFVar1 == (FARPROC)0x0) {
    local_10 = &param_1;
    local_c = 4;
    local_14 = 4;
    FUN_00557c08(&local_14,&local_10);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3,param_4);
    (*pFVar1)();
  }
  return;
}

