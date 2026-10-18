/* Entry: 0x00414540; symbol: operator=; body bytes: 60 */

/* Library Function - Single Match
    public: class std::shared_ptr<class __ExceptionPtr> & __thiscall std::shared_ptr<class
   __ExceptionPtr>::operator=(class std::shared_ptr<class __ExceptionPtr> const &)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release */

shared_ptr<class___ExceptionPtr> * __thiscall
std::shared_ptr<class___ExceptionPtr>::operator=
          (shared_ptr<class___ExceptionPtr> *this,shared_ptr<class___ExceptionPtr> *param_1)

{
  undefined1 local_38 [40];
  void *local_10;
  void *local_c;
  shared_ptr<class___ExceptionPtr> *local_8;
  
  local_8 = this;
  local_10 = function<>(local_38,(int)param_1);
  local_c = local_10;
  swap(local_10,local_8);
  FUN_00414400(local_38);
  return local_8;
}

