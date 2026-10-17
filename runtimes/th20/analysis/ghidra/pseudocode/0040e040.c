/* Entry: 0x0040e040; symbol: _System_error_message; body bytes: 44 */

/* Library Function - Single Match
    public: __thiscall std::_System_error_message::_System_error_message(unsigned long)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

_System_error_message * __thiscall
std::_System_error_message::_System_error_message(_System_error_message *this,ulong param_1)

{
  undefined4 uVar1;
  
  *(undefined4 *)this = 0;
  uVar1 = FUN_00540f5f(param_1,(int *)this);
  *(undefined4 *)(this + 4) = uVar1;
  return this;
}

