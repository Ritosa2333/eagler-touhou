
// block 00491b36
00491b36  mov        edi, edi
00491b38  push       ebp
00491b39  mov        ebp, esp
00491b3b  push       esi
00491b3c  cld        
00491b3d  mov        esi, dword ptr [ebp + 0xc]
00491b40  mov        ecx, dword ptr [esi + 8]
00491b43  xor        ecx, esi
00491b45  call       0x46c932

// block 00491b4a
00491b4a  push       0
00491b4c  push       esi
00491b4d  push       dword ptr [esi + 0x14]
00491b50  push       dword ptr [esi + 0xc]
00491b53  push       0
00491b55  push       dword ptr [ebp + 0x10]
00491b58  push       dword ptr [esi + 0x10]
00491b5b  push       dword ptr [ebp + 8]
00491b5e  call       0x493064

// block 00491b63
00491b63  add        esp, 0x20
00491b66  pop        esi
00491b67  pop        ebp
00491b68  ret        
