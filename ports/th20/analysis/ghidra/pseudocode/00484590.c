/* Entry: 0x00484590; symbol: Decref; body bytes: 48 */

/* Library Function - Multiple Matches With Same Base Name
    public: void __thiscall std::_Ref_count_base::_Decref(void)
    public: void __thiscall std::tr1::_Ref_count_base::_Decref(void)
   
   Libraries: Visual Studio 2010 Debug, Visual Studio 2012 Debug, Visual Studio 2012 Release */

void __fastcall Decref(int *param_1)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = param_1 + 1;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    (**(code **)*param_1)();
    Decwref(param_1);
  }
  return;
}

