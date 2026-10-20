
// block 00452580
00452580  sub        esp, 0x10
00452583  fld        dword ptr [0x4a3d78]
00452589  push       ebx
0045258a  mov        ebx, dword ptr [ecx + 0x18]
0045258d  fstp       dword ptr [esp + 4]
00452591  fld        dword ptr [0x4a3e68]
00452597  shl        ebx, 0x18
0045259a  or         ebx, dword ptr [ecx + 0x20]
0045259d  fstp       dword ptr [esp + 8]
004525a1  fld        dword ptr [0x4a3ebc]
004525a7  push       edi
004525a8  fstp       dword ptr [esp + 0x10]
004525ac  lea        edi, [esp + 8]
004525b0  fld        dword ptr [0x4a3eb8]
004525b6  fstp       dword ptr [esp + 0x14]
004525ba  call       0x451f30

// block 004525bf
004525bf  pop        edi
004525c0  mov        eax, 1
004525c5  pop        ebx
004525c6  add        esp, 0x10
004525c9  ret        
