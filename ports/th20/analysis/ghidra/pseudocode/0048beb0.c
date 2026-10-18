/* Entry: 0x0048beb0; symbol: `scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Single Match
    public: virtual void * __thiscall Concurrency::details::CachedTransmogrifiedPrimary::`scalar
   deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::details::CachedTransmogrifiedPrimary::_scalar_deleting_destructor_
          (CachedTransmogrifiedPrimary *this,uint param_1)

{
  FUN_0048b870((int)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

