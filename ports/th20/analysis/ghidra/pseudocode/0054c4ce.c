/* Entry: 0x0054c4ce; symbol: FID_conflict:operator(); body bytes: 44 */

/* Library Function - Multiple Matches With Different Base Names
    public: bool __thiscall <lambda_136dcf69874064575ae1b89ff2a94cd1>::operator()(void)const 
    public: bool __thiscall <lambda_448605b25275ab87d3c5d4aa7a38a0aa>::operator()(void)const 
    public: bool __thiscall <lambda_46698efe93618fff5274d84d009d2be1>::operator()(void)const 
    public: bool __thiscall <lambda_5f3f4d49f9676d963a9a63518de6904a>::operator()(void)const 
     9 names - too many to list
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __fastcall FID_conflict_operator__(undefined4 *param_1)

{
  __crt_strtox::
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
            ((input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
              *)*param_1,*(wchar_t *)param_1[1]);
  *(undefined2 *)param_1[1] = 0;
  restore_state((void *)*param_1,*(int *)param_1[2],((int *)param_1[2])[1]);
  return;
}

