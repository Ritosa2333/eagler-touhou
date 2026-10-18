/* Entry: 0x00498f70; symbol: base; body bytes: 26 */

/* Library Function - Multiple Matches With Same Base Name
    public: class std::_Vector_iterator<class std::_Vector_val<struct std::_Simple_types<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<unsigned char> > > > > __thiscall
   std::move_iterator<class std::_Vector_iterator<class std::_Vector_val<struct
   std::_Simple_types<class std::shared_ptr<struct Concurrency::details::_Task_impl<unsigned char> >
   > > > >::base(void)const 
    public: class std::_Vector_iterator<class std::_Vector_val<struct std::_Simple_types<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > > > > > __thiscall std::move_iterator<class
   std::_Vector_iterator<class std::_Vector_val<struct std::_Simple_types<class
   std::shared_ptr<struct Concurrency::details::_Task_impl<struct std::pair<unsigned char,class
   Concurrency::details::_CancellationTokenState *> > > > > > >::base(void)const 
   
   Library: Visual Studio 2012 Release */

undefined4 * __thiscall base(void *this,undefined4 *param_1)

{
                    /* WARNING: Load size is inaccurate */
  *param_1 = *this;
  return param_1;
}

