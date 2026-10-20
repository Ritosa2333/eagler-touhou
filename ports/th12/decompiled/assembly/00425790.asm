
// block 00425790
00425790  fldz       
00425792  push       esi
00425793  mov        esi, dword ptr [esp + 8]
00425797  mov        dword ptr [eax + 0x1e0], esi
0042579d  movzx      esi, byte ptr [esp + 0xc]
004257a2  mov        dword ptr [eax + 0x1e4], esi
004257a8  mov        esi, dword ptr [edx]
004257aa  mov        dword ptr [eax + 0x1ac], esi
004257b0  mov        edx, dword ptr [edx + 4]
004257b3  mov        dword ptr [eax + 0x1b0], edx
004257b9  mov        edx, dword ptr [ecx]
004257bb  mov        dword ptr [eax + 0x1b4], edx
004257c1  mov        ecx, dword ptr [ecx + 4]
004257c4  mov        dword ptr [eax + 0x1b8], ecx
004257ca  mov        ecx, dword ptr [eax + 0x1dc]
004257d0  xor        edx, edx
004257d2  pop        esi
004257d3  test       cl, 1
004257d6  jne        0x425801

// block 004257d8
004257d8  or         ecx, 1
004257db  fst        dword ptr [eax + 0x1d4]
004257e1  mov        dword ptr [eax + 0x1d0], edx
004257e7  mov        dword ptr [eax + 0x1cc], 0xfff0bdc1
004257f1  mov        dword ptr [eax + 0x1d8], 0x4b2ed0
004257fb  mov        dword ptr [eax + 0x1dc], ecx

// block 00425801
00425801  fstp       dword ptr [eax + 0x1d4]
00425807  mov        dword ptr [eax + 0x1d0], edx
0042580d  mov        dword ptr [eax + 0x1cc], 0xffffffff
00425817  ret        8
