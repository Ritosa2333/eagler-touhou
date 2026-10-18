/* Entry: 0x00536910; symbol: Do_call; body bytes: 41 */

/* Library Function - Multiple Matches With Same Base Name
    private: virtual bool __thiscall std::_Func_impl_no_alloc<bool (__cdecl*)(enum
   Concurrency::agent_status const &),bool,enum Concurrency::agent_status const &>::_Do_call(enum
   Concurrency::agent_status const &)
    private: virtual void __thiscall std::_Func_impl_no_alloc<class
   <lambda_0b644b0099f9cbc573e00435de85ed83>,void,class Concurrency::message<unsigned int>
   *>::_Do_call(class Concurrency::message<unsigned int> * &&)
    private: virtual void __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::target_block<class Concurrency::multi_link_registry<class
   Concurrency::ISource<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::enable_batched_processing(void)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned
   int> *>::_Do_call(class Concurrency::message<unsigned int> * &&)
    private: virtual void __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::multi_link_registry<class
   Concurrency::ITarget<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::initialize_source(class Concurrency::Scheduler *,class Concurrency::ScheduleGroup
   *)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned int> *>::_Do_call(class
   Concurrency::message<unsigned int> * &&)
     13 names - too many to list
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall Do_call(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(param_1);
  FUN_00535bf0((void *)((int)this + 4),puVar1);
  return;
}

