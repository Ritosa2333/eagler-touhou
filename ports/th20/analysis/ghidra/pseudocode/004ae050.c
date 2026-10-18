/* Entry: 0x004ae050; symbol: _Reset<>; body bytes: 90 */

/* Library Function - Multiple Matches With Same Base Name
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<unsigned int>
   *>::_Reset<class `protected: void __thiscall Concurrency::target_block<class
   Concurrency::multi_link_registry<class Concurrency::ISource<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int>
   >::enable_batched_processing(void)'::`2'::<lambda_1> >(class `protected: void __thiscall
   Concurrency::target_block<class Concurrency::multi_link_registry<class
   Concurrency::ISource<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::enable_batched_processing(void)'::`2'::<lambda_1> &&)
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<unsigned int>
   *>::_Reset<class `protected: void __thiscall Concurrency::source_block<class
   Concurrency::multi_link_registry<class Concurrency::ITarget<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1> >(class
   `protected: void __thiscall Concurrency::source_block<class
   Concurrency::multi_link_registry<class Concurrency::ITarget<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1> &&)
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<enum
   Concurrency::agent_status> *>::_Reset<class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::multi_link_registry<class Concurrency::ITarget<enum
   Concurrency::agent_status> >,class Concurrency::ordered_message_processor<enum
   Concurrency::agent_status> >::initialize_source(class Concurrency::Scheduler *,class
   Concurrency::ScheduleGroup *)'::`2'::<lambda_1> >(class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::multi_link_registry<class Concurrency::ITarget<enum
   Concurrency::agent_status> >,class Concurrency::ordered_message_processor<enum
   Concurrency::agent_status> >::initialize_source(class Concurrency::Scheduler *,class
   Concurrency::ScheduleGroup *)'::`2'::<lambda_1> &&)
    protected: void __thiscall std::_Func_class<void,class Concurrency::message<unsigned int>
   *>::_Reset<class `protected: void __thiscall Concurrency::source_block<class
   Concurrency::single_link_registry<class Concurrency::ITarget<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1> >(class
   `protected: void __thiscall Concurrency::source_block<class
   Concurrency::single_link_registry<class Concurrency::ITarget<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1> &&)
     5 names - too many to list
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall _Reset<>(void *this,undefined4 param_1)

{
  char cVar1;
  void *this_00;
  undefined4 uVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  
  cVar1 = _Test_callable<>();
  if (cVar1 != '\0') {
    this_00 = (void *)move<>(8,this);
    uVar2 = move<>(param_1);
    p_Var3 = FUN_004ac490(this_00,uVar2);
    Set(this,p_Var3);
  }
  return;
}

