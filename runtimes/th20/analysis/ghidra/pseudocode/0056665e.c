/* Entry: 0x0056665e; symbol: ___acrt_LCMapStringW; body bytes: 62 */

/* Library Function - Single Match
    ___acrt_LCMapStringW
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
___acrt_LCMapStringW
          (void *this,ushort *param_1,DWORD param_2,LPCWSTR param_3,void *param_4,LPWSTR param_5,
          int param_6)

{
  void *pvVar1;
  void *pvVar2;
  
  pvVar2 = param_4;
  if (0 < (int)param_4) {
    this = param_4;
    pvVar1 = (void *)FUN_00548430(param_3,(uint)param_4);
    pvVar2 = (void *)((int)pvVar1 + 1);
    if ((int)param_4 <= (int)pvVar1) {
      pvVar2 = pvVar1;
    }
  }
  ___acrt_LCMapStringEx_36(this,param_1,param_2,param_3,(int)pvVar2,param_5,param_6,0,0,0);
  return;
}

