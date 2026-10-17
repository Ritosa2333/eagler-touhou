/* Entry: 0x00539db0; symbol: `vector_deleting_destructor'; body bytes: 163 */

/* Library Function - Single Match
    public: void * __thiscall Concurrency::details::SchedulerBase::NumaInformation::`vector deleting
   destructor'(unsigned int)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void * __thiscall
Concurrency::details::SchedulerBase::NumaInformation::_vector_deleting_destructor_
          (NumaInformation *this,uint param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if ((param_1 & 2) == 0) {
    FUN_00539cf0((int *)this);
    if ((param_1 & 1) != 0) {
      FUN_0054278d(this);
    }
  }
  else {
    FUN_00542f23(this,0x10,*(int *)(this + -4),FUN_00539cf0);
    if ((param_1 & 1) != 0) {
      FUN_0054303b(this + -4);
    }
    this = this + -4;
  }
  *unaff_FS_OFFSET = local_10;
  return this;
}

