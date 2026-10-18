/* Entry: 0x0055e6c6; symbol: ~argument_list<>; body bytes: 37 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall `anonymous namespace'::argument_list<char>::~argument_list<char>(void)
    public: __thiscall `anonymous namespace'::argument_list<char>::~argument_list<char>(void)
    public: __thiscall `anonymous namespace'::argument_list<wchar_t>::~argument_list<wchar_t>(void)
    public: __thiscall `anonymous namespace'::argument_list<wchar_t>::~argument_list<wchar_t>(void)
   
   Library: Visual Studio 2015 Release */

void __fastcall ~argument_list<>(undefined4 *param_1)

{
  undefined4 *puVar1;
  
  for (puVar1 = (undefined4 *)*param_1; puVar1 != (undefined4 *)param_1[1]; puVar1 = puVar1 + 1) {
    FUN_00557ba0((LPVOID)*puVar1);
  }
  FUN_00557ba0((LPVOID)*param_1);
  return;
}

