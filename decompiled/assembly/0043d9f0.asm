
// block 0043d9f0
0043d9f0  sub        esp, 0xc
0043d9f3  fldz       
0043d9f5  push       ebx
0043d9f6  fst        dword ptr [esp + 4]
0043d9fa  push       esi
0043d9fb  mov        esi, dword ptr [0x4b43b8]
0043da01  fst        dword ptr [esp + 0xc]
0043da05  push       0x4a1628
0043da0a  fstp       dword ptr [esp + 0x14]
0043da0e  lea        ebx, [esp + 0xc]
0043da12  call       0x401720

// block 0043da17
0043da17  mov        eax, dword ptr [edi + 0x30]
0043da1a  add        esp, 4
0043da1d  sub        eax, 1
0043da20  jne        0x43dab5

// block 0043da26
0043da26  fld        dword ptr [0x4a3f80]
0043da2c  mov        eax, dword ptr [edi + 0x10c]
0043da32  mov        esi, dword ptr [0x4b43b8]
0043da38  fstp       dword ptr [esp + 8]
0043da3c  fld        dword ptr [0x4a3e68]
0043da42  push       eax
0043da43  fstp       dword ptr [esp + 0x10]
0043da47  push       0x4a1634
0043da4c  fldz       
0043da4e  lea        ebx, [esp + 0x10]
0043da52  fstp       dword ptr [esp + 0x18]
0043da56  call       0x401720

// block 0043da5b
0043da5b  fld        dword ptr [0x4a3f7c]
0043da61  mov        esi, dword ptr [0x4b43b8]
0043da67  fstp       dword ptr [esp + 0x14]
0043da6b  push       0x49f878
0043da70  call       0x401720

// block 0043da75
0043da75  fld        dword ptr [0x4a3d78]
0043da7b  mov        esi, dword ptr [0x4b43b8]
0043da81  fstp       dword ptr [esp + 0x14]
0043da85  fild       dword ptr [edi + 0x34]
0043da88  push       0x49f880
0043da8d  fmul       qword ptr [0x4a3e40]
0043da93  fadd       qword ptr [0x4a3d68]
0043da99  fstp       dword ptr [esp + 0x1c]
0043da9d  call       0x401720

// block 0043daa2
0043daa2  mov        ecx, dword ptr [0x4b43b8]
0043daa8  add        esp, 0x10
0043daab  mov        dword ptr [ecx + 0x18f80], 0xffffffff

// block 0043dab5
0043dab5  pop        esi
0043dab6  mov        eax, 1
0043dabb  pop        ebx
0043dabc  add        esp, 0xc
0043dabf  ret        
