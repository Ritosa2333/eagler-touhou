/* Entry: 0x004b6900; symbol: Copy; body bytes: 47 */

/* Library Function - Multiple Matches With Same Base Name
    private: virtual class std::_Func_base<bool,enum Concurrency::agent_status const &> * __thiscall
   std::_Func_impl_no_alloc<bool (__cdecl*)(enum Concurrency::agent_status const &),bool,enum
   Concurrency::agent_status const &>::_Copy(void *)const 
    private: virtual class std::_Func_base<void,class Concurrency::message<unsigned int> *> *
   __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::target_block<class Concurrency::multi_link_registry<class
   Concurrency::ISource<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::enable_batched_processing(void)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned
   int> *>::_Copy(void *)const 
    private: virtual class std::_Func_base<void,class Concurrency::message<unsigned int> *> *
   __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::multi_link_registry<class
   Concurrency::ITarget<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::initialize_source(class Concurrency::Scheduler *,class Concurrency::ScheduleGroup
   *)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned int> *>::_Copy(void *)const 
    private: virtual class std::_Func_base<void,class Concurrency::message<enum
   Concurrency::agent_status> *> * __thiscall std::_Func_impl_no_alloc<class `protected: void
   __thiscall Concurrency::source_block<class Concurrency::multi_link_registry<class
   Concurrency::ITarget<enum Concurrency::agent_status> >,class
   Concurrency::ordered_message_processor<enum Concurrency::agent_status> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1>,void,class
   Concurrency::message<enum Concurrency::agent_status> *>::_Copy(void *)const 
     6 names - too many to list
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall Copy(void *this,undefined4 param_1)

{
  void *this_00;
  
  this_00 = (void *)move<>(8,param_1);
  FUN_004ac6c0(this_00,(int)this + 4);
  return;
}

