/* Entry: 0x0040b190; symbol: FUN_0040b190; body bytes: 60 */

_Callable_base<bool_(__cdecl*const)(enum_Concurrency::agent_status_const&),0> * __thiscall
FUN_0040b190(void *this,undefined4 param_1,undefined4 param_2)

{
  _func_bool_agent_status_ptr **pp_Var1;
  
  FUN_0040b120(this);
  pp_Var1 = (_func_bool_agent_status_ptr **)move<>(param_2);
  std::_Callable_base<bool_(__cdecl*const)(enum_Concurrency::agent_status_const&),0>::
  _Callable_base<bool_(__cdecl*const)(enum_Concurrency::agent_status_const&),0><bool_(__cdecl*const&)(enum_Concurrency::agent_status_const&)>
            ((_Callable_base<bool_(__cdecl*const)(enum_Concurrency::agent_status_const&),0> *)this,
             pp_Var1);
  return (_Callable_base<bool_(__cdecl*const)(enum_Concurrency::agent_status_const&),0> *)this;
}

