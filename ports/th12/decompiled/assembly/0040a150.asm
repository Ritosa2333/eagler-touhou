
// block 0040a150
0040a150  push       esi
0040a151  mov        esi, dword ptr [eax + ecx*4 + 0x14]
0040a155  test       esi, esi
0040a157  je         0x40a1e0

// block 0040a15d
0040a15d  lea        ecx, [ecx]

// block 0040a160
0040a160  fld        dword ptr [esi + 0x4bc]
0040a166  fadd       qword ptr [0x4a3d70]
0040a16c  fadd       qword ptr [0x4a3d28]
0040a172  fstp       dword ptr [esi + 0x42c]
0040a178  fld        dword ptr [esi + 0x4c0]
0040a17e  fadd       qword ptr [0x4a3d68]
0040a184  fstp       dword ptr [esi + 0x430]
0040a18a  fld        dword ptr [esi + 0x4c4]
0040a190  fstp       dword ptr [esi + 0x434]
0040a196  test       dword ptr [esi + 0x484], 0x20000000
0040a1a0  je         0x40a1c7

// block 0040a1a2
0040a1a2  fld        dword ptr [0x4a3e08]
0040a1a8  sub        esp, 8
0040a1ab  fstp       dword ptr [esp + 4]
0040a1af  fld        dword ptr [esi + 0x4d8]
0040a1b5  fstp       dword ptr [esp]
0040a1b8  call       0x464640

// block 0040a1bd
0040a1bd  or         dword ptr [esi + 0x484], 4
0040a1c4  fstp       dword ptr [esi + 0x34]

// block 0040a1c7
0040a1c7  mov        edx, dword ptr [0x4ce8cc]
0040a1cd  lea        eax, [esi + 8]
0040a1d0  push       edx
0040a1d1  call       0x45c900

// block 0040a1d6
0040a1d6  mov        esi, dword ptr [esi + 0x538]
0040a1dc  test       esi, esi
0040a1de  jne        0x40a160

// block 0040a1e0
0040a1e0  mov        eax, 1
0040a1e5  pop        esi
0040a1e6  ret        
