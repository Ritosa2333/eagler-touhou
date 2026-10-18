/* Entry: 0x005414f2; symbol: do_allocate; body bytes: 36 */

/* Library Function - Single Match
    private: virtual void * __thiscall
   std::pmr::_Aligned_new_delete_resource_impl::do_allocate(unsigned int,unsigned int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void * __thiscall
std::pmr::_Aligned_new_delete_resource_impl::do_allocate
          (_Aligned_new_delete_resource_impl *this,uint param_1,uint param_2)

{
  void *pvVar1;
  
  if (param_2 < 9) {
    pvVar1 = operator_new(param_1);
  }
  else {
    pvVar1 = (void *)FUN_00542e42(param_1,param_2);
  }
  return pvVar1;
}

