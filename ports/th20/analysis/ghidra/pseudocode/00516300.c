/* Entry: 0x00516300; symbol: ~ios_base; body bytes: 74 */

/* Library Function - Single Match
    public: virtual __thiscall std::ios_base::~ios_base(void)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall std::ios_base::~ios_base(ios_base *this)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *(undefined ***)this = &PTR__scalar_deleting_destructor__00574d28;
  _Ios_base_dtor(this);
  *unaff_FS_OFFSET = local_10;
  return;
}

