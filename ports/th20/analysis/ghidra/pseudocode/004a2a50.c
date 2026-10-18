/* Entry: 0x004a2a50; symbol: make_pair<>; body bytes: 59 */

/* Library Function - Multiple Matches With Same Base Name
    struct std::pair<double,double> __cdecl std::make_pair<double,double &>(double &&,double &)
    struct std::pair<long double,long double> __cdecl std::make_pair<long double,long double &>(long
   double &&,long double &)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void * __cdecl make_pair<>(void *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = move<>(param_3);
  uVar2 = move<>(param_2);
  FUN_004a1410(param_1,uVar2,uVar1);
  return param_1;
}

