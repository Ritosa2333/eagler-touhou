/* Entry: 0x00516750; symbol: `scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Single Match
    public: virtual void * __thiscall std::ios_base::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall std::ios_base::_scalar_deleting_destructor_(ios_base *this,uint param_1)

{
  ~ios_base(this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

