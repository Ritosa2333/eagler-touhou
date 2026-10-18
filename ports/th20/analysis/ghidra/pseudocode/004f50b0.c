/* Entry: 0x004f50b0; symbol: FID_conflict:evaluation_error; body bytes: 37 */

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

undefined4 * __thiscall FID_conflict_evaluation_error(void *this,exception *param_1)

{
  FUN_0040e470(this,param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__005726cc;
  return (undefined4 *)this;
}

