
// block 00452680
00452680  sub        esp, 0x10
00452683  fld        dword ptr [0x4a3d78]
00452689  mov        eax, dword ptr [ecx + 0x18]
0045268c  fstp       dword ptr [esp]
0045268f  push       ebx
00452690  fld        dword ptr [0x4a3e68]
00452696  mov        ebx, dword ptr [ecx + 0x24]
00452699  fstp       dword ptr [esp + 8]
0045269d  and        ebx, 0xffffff
004526a3  fld        dword ptr [0x4a3ebc]
004526a9  shl        eax, 0x18
004526ac  fstp       dword ptr [esp + 0xc]
004526b0  push       edi
004526b1  fld        dword ptr [0x4a3eb8]
004526b7  or         ebx, eax
004526b9  lea        edi, [esp + 8]
004526bd  fstp       dword ptr [esp + 0x14]
004526c1  call       0x451f30

// block 004526c6
004526c6  pop        edi
004526c7  mov        eax, 1
004526cc  pop        ebx
004526cd  add        esp, 0x10
004526d0  ret        
