/* Entry: 0x0040e000; symbol: _System_error; body bytes: 55 */

/* Library Function - Multiple Matches With Different Base Names
    public: __thiscall std::_System_error::_System_error(class std::_System_error const &)
    public: __thiscall std::system_error::system_error(class std::system_error const &)
   
   Library: Visual Studio */

_System_error * __thiscall
std::_System_error::_System_error(_System_error *this,_System_error *param_1)

{
  undefined4 uVar1;
  
  FUN_0040e470(this,(exception *)param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__0056c4e8;
  uVar1 = *(undefined4 *)(param_1 + 0x10);
  *(undefined4 *)(this + 0xc) = *(undefined4 *)(param_1 + 0xc);
  *(undefined4 *)(this + 0x10) = uVar1;
  return this;
}

