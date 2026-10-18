/* Entry: 0x00515d60; symbol: FID_conflict:evaluation_error; body bytes: 37 */

/* Library Function - Multiple Matches With Different Base Names
    public: __thiscall CTypedPtrList<class CList<void *,void *>,struct COleControlSiteOrWnd
   *>::CTypedPtrList<class CList<void *,void *>,struct COleControlSiteOrWnd *>(int)
    public: __thiscall CTypedPtrList<class CPtrList,struct COleControlSiteOrWnd
   *>::CTypedPtrList<class CPtrList,struct COleControlSiteOrWnd *>(int)
    public: __thiscall __non_rtti_object::__non_rtti_object(class __non_rtti_object const &)
    public: __thiscall std::__non_rtti_object::__non_rtti_object(class std::__non_rtti_object const
   &)
     21 names - too many to list
   
   Library: Visual Studio */

system_error * __thiscall FID_conflict_evaluation_error(void *this,system_error *param_1)

{
  std::system_error::system_error((system_error *)this,param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00574d1c;
  return (system_error *)this;
}

