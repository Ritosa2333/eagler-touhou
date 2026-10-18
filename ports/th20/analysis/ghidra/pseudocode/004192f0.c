/* Entry: 0x004192f0; symbol: `scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Single Match
    public: void * __thiscall std::shared_ptr<class __ExceptionPtr>::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2015 Release, Visual Studio 2017 Debug, Visual
   Studio 2017 Release */

void * __thiscall
std::shared_ptr<class___ExceptionPtr>::_scalar_deleting_destructor_
          (shared_ptr<class___ExceptionPtr> *this,uint param_1)

{
  ~shared_ptr<class___ExceptionPtr>(this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

