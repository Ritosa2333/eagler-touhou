/* Entry: 0x0054c408; symbol: input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>; body bytes: 68 */

/* Library Function - Single Match
    public: __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >(class
   __crt_stdio_input::string_input_adapter<wchar_t> const &,unsigned __int64,wchar_t const *
   const,struct __crt_locale_pointers * const,char * const)
   
   Library: Visual Studio 2019 Release */

input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> * __thiscall
__crt_stdio_input::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
::input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
          (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> *this,
          string_input_adapter<wchar_t> *param_1,__uint64 param_2,wchar_t *param_3,
          __crt_locale_pointers *param_4,char *param_5)

{
  undefined4 in_stack_00000008;
  
  *(undefined4 *)this = in_stack_00000008;
  *(undefined4 *)(this + 4) = (undefined4)param_2;
  *(undefined4 *)(this + 8) = *(undefined4 *)param_1;
  *(undefined4 *)(this + 0xc) = *(undefined4 *)(param_1 + 4);
  *(undefined4 *)(this + 0x10) = *(undefined4 *)(param_1 + 8);
  FUN_0054c35c(this + 0x18,in_stack_00000008,(undefined4)param_2,param_2._4_4_);
  *(undefined4 *)(this + 0x58) = 0;
  *(wchar_t **)(this + 0x50) = param_3;
  *(__crt_locale_pointers **)(this + 0x54) = param_4;
  return this;
}

