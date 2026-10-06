
// block 0041a9b0
0041a9b0  mov        ecx, dword ptr [eax + 0x174c]
0041a9b6  fld        dword ptr [esp + 8]
0041a9ba  mov        eax, dword ptr [ecx + 4]
0041a9bd  mov        ecx, dword ptr [esp + 4]
0041a9c1  mov        edx, dword ptr [eax + 4]
0041a9c4  movzx      edx, word ptr [edx + 8]
0041a9c8  push       esi
0041a9c9  mov        esi, 1
0041a9ce  shl        esi, cl
0041a9d0  test       esi, edx
0041a9d2  pop        esi
0041a9d3  je         0x41a9de

// block 0041a9d5
0041a9d5  push       ecx
0041a9d6  fstp       dword ptr [esp]
0041a9d9  call       0x468fe0

// block 0041a9de
0041a9de  fstp       dword ptr [esp + 8]
0041a9e2  fld        dword ptr [esp + 8]
0041a9e6  ret        8
