/* Entry: 0x00541516; symbol: do_deallocate; body bytes: 45 */

/* Library Function - Single Match
    private: virtual void __thiscall std::pmr::_Aligned_new_delete_resource_impl::do_deallocate(void
   * const,unsigned int,unsigned int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
std::pmr::_Aligned_new_delete_resource_impl::do_deallocate
          (_Aligned_new_delete_resource_impl *this,void *param_1,uint param_2,uint param_3)

{
  if (param_3 < 9) {
    FUN_0054278d(param_1);
  }
  else {
    operator_delete(param_1,param_2,param_3);
  }
  return;
}

