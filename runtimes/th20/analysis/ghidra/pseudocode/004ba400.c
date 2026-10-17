/* Entry: 0x004ba400; symbol: `scalar_deleting_destructor'; body bytes: 49 */

/* Library Function - Single Match
    public: virtual void * __thiscall Concurrency::source_block<class
   Concurrency::single_link_registry<class Concurrency::ITarget<unsigned int> >,class
   Concurrency::ordered_message_processor<unsigned int> >::`scalar deleting destructor'(unsigned
   int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::
source_block<class_Concurrency::single_link_registry<class_Concurrency::ITarget<unsigned_int>_>,class_Concurrency::ordered_message_processor<unsigned_int>_>
::_scalar_deleting_destructor_
          (source_block<class_Concurrency::single_link_registry<class_Concurrency::ITarget<unsigned_int>_>,class_Concurrency::ordered_message_processor<unsigned_int>_>
           *this,uint param_1)

{
  FUN_004b9ef0((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

