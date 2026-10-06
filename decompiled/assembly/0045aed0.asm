
// block 0045aed0
0045aed0  fld        dword ptr [esp + 4]
0045aed4  fld        st(0)
0045aed6  fld        dword ptr [esp + 0x10]
0045aeda  fld        st(0)
0045aedc  fmulp      st(2)
0045aede  fld        dword ptr [esp + 8]
0045aee2  fld        st(0)
0045aee4  fld        dword ptr [esp + 0xc]
0045aee8  fld        st(0)
0045aeea  fmulp      st(2)
0045aeec  fxch       st(4)
0045aeee  fsubrp     st(1)
0045aef0  fadd       dword ptr [esp + 0x14]
0045aef4  fstp       dword ptr [eax]
0045aef6  fxch       st(3)
0045aef8  fmulp      st(2)
0045aefa  fmulp      st(2)
0045aefc  faddp      st(1)
0045aefe  fadd       dword ptr [esp + 0x18]
0045af02  fstp       dword ptr [eax + 4]
0045af05  ret        0x18
