/* Entry: 0x0046b4e0; symbol: `scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Single Match
    public: void * __thiscall Concurrency::details::_Condition_variable::`scalar deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::details::_Condition_variable::_scalar_deleting_destructor_
          (_Condition_variable *this,uint param_1)

{
  ~input_processor<>((int)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

