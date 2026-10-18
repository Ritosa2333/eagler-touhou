/* Entry: 0x004a0c80; symbol: Move; body bytes: 104 */

/* Library Function - Multiple Matches With Same Base Name
    private: virtual class std::_Func_base<void,class Concurrency::message<unsigned int> *> *
   __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::target_block<class Concurrency::multi_link_registry<class
   Concurrency::ISource<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::enable_batched_processing(void)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned
   int> *>::_Move(void *)
    private: virtual class std::_Func_base<void,class Concurrency::message<unsigned int> *> *
   __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::multi_link_registry<class
   Concurrency::ITarget<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::initialize_source(class Concurrency::Scheduler *,class Concurrency::ScheduleGroup
   *)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned int> *>::_Move(void *)
    private: virtual class std::_Func_base<void,class Concurrency::message<enum
   Concurrency::agent_status> *> * __thiscall std::_Func_impl_no_alloc<class `protected: void
   __thiscall Concurrency::source_block<class Concurrency::multi_link_registry<class
   Concurrency::ITarget<enum Concurrency::agent_status> >,class
   Concurrency::ordered_message_processor<enum Concurrency::agent_status> >::initialize_source(class
   Concurrency::Scheduler *,class Concurrency::ScheduleGroup *)'::`2'::<lambda_1>,void,class
   Concurrency::message<enum Concurrency::agent_status> *>::_Move(void *)
    private: virtual class std::_Func_base<void,class Concurrency::message<unsigned int> *> *
   __thiscall std::_Func_impl_no_alloc<class `protected: void __thiscall
   Concurrency::source_block<class Concurrency::single_link_registry<class
   Concurrency::ITarget<unsigned int> >,class Concurrency::ordered_message_processor<unsigned int>
   >::initialize_source(class Concurrency::Scheduler *,class Concurrency::ScheduleGroup
   *)'::`2'::<lambda_1>,void,class Concurrency::message<unsigned int> *>::_Move(void *)
     5 names - too many to list
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall Move(void *this,undefined4 param_1)

{
  void *this_00;
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this_00 = (void *)move<>(8,param_1);
  uVar1 = move<>((int)this + 4);
  FID_conflict__Func_impl_no_alloc<class__protected:_void___thiscall_Concurrency::target_block<class_Concurrency::multi_link_registry<class_Concurrency::ISource<unsigned_int>_>,class_Concurrency::ordered_message_processor<unsigned_int>_>::initialize_target(class_Concurrency::Scheduler*,class_Concurrency::ScheduleGroup*)_::_2_::<lambda_1>,void,class_Concurrency::message<unsigned_int>*><class__protected:_void___thiscall_Concurrency::target_block<class_Concurrency::multi_link_registry<class_Concurrency::ISource<unsigned_int>_>,class_Concurrency::ordered_message_processor<unsigned_int>_>::initialize_target(class_Concurrency::Scheduler*,class_Concurrency::ScheduleGroup*)_::_2_::<lambda_1>,0>
            (this_00,uVar1);
  *unaff_FS_OFFSET = local_10;
  return;
}

