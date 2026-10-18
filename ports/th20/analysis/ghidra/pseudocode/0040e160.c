/* Entry: 0x0040e160; symbol: exception; body bytes: 60 */

/* Library Function - Single Match
    public: __thiscall std::exception::exception(class std::exception const &)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

exception * __thiscall std::exception::exception(exception *this,exception *param_1)

{
  *(undefined ***)this = &PTR_FUN_0056c47c;
  *(undefined4 *)(this + 4) = 0;
  *(undefined4 *)(this + 8) = 0;
  ___std_exception_copy((int *)(param_1 + 4),(int *)(this + 4));
  return this;
}

