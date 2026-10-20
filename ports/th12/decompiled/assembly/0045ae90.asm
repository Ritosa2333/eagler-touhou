
// block 0045ae90
0045ae90  fld        dword ptr [esp + 4]
0045ae94  fld        st(0)
0045ae96  fld        dword ptr [esp + 0x10]
0045ae9a  fld        st(0)
0045ae9c  fmulp      st(2)
0045ae9e  fld        dword ptr [esp + 8]
0045aea2  fld        st(0)
0045aea4  fld        dword ptr [esp + 0xc]
0045aea8  fld        st(0)
0045aeaa  fmulp      st(2)
0045aeac  fxch       st(4)
0045aeae  fsubrp     st(1)
0045aeb0  fadd       dword ptr [esp + 0x14]
0045aeb4  fstp       dword ptr [eax]
0045aeb6  fxch       st(3)
0045aeb8  fmulp      st(2)
0045aeba  fmulp      st(2)
0045aebc  faddp      st(1)
0045aebe  fadd       dword ptr [esp + 0x18]
0045aec2  fstp       dword ptr [eax + 4]
0045aec5  ret        0x18
