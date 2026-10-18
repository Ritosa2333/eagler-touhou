/* Entry: 0x004c9190; symbol: `scalar_deleting_destructor'; body bytes: 49 */

/* Library Function - Single Match
    public: virtual void * __thiscall Concurrency::details::ContextBase::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::details::ContextBase::_scalar_deleting_destructor_(ContextBase *this,uint param_1)

{
  ~ContextBase(this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

