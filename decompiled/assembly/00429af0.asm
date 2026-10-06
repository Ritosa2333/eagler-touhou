
// block 00429af0
00429af0  push       esi
00429af1  mov        esi, ecx
00429af3  fld        dword ptr [esi + 0x50]
00429af6  push       edi
00429af7  fadd       qword ptr [0x4a3d70]
00429afd  sub        esp, 8
00429b00  lea        edi, [esi + 0x63c]
00429b06  fadd       qword ptr [0x4a3d28]
00429b0c  fstp       dword ptr [esi + 0xa60]
00429b12  fld        dword ptr [esi + 0x54]
00429b15  fadd       qword ptr [0x4a3d68]
00429b1b  fstp       dword ptr [esi + 0xa64]
00429b21  fld        dword ptr [esi + 0x58]
00429b24  fstp       dword ptr [esi + 0xa68]
00429b2a  fld        dword ptr [0x4a3e08]
00429b30  fstp       dword ptr [esp + 4]
00429b34  fld        dword ptr [esi + 0x68]
00429b37  fstp       dword ptr [esp]
00429b3a  call       0x464640

// block 00429b3f
00429b3f  mov        eax, dword ptr [0x4ce8cc]
00429b44  fstp       dword ptr [edi + 0x2c]
00429b47  or         dword ptr [edi + 0x47c], 4
00429b4e  push       eax
00429b4f  mov        eax, edi
00429b51  call       0x45c900

// block 00429b56
00429b56  fldz       
00429b58  fcomp      dword ptr [esi + 0x78]
00429b5b  fnstsw     ax
00429b5d  test       ah, 0x44
00429b60  jp         0x429ba1

// block 00429b62
00429b62  fld        dword ptr [esi + 0x50]
00429b65  mov        ecx, dword ptr [0x4ce8cc]
00429b6b  fadd       qword ptr [0x4a3d70]
00429b71  lea        eax, [esi + 0xaf0]
00429b77  push       ecx
00429b78  fadd       qword ptr [0x4a3d28]
00429b7e  fstp       dword ptr [esi + 0xf14]
00429b84  fld        dword ptr [esi + 0x54]
00429b87  fadd       qword ptr [0x4a3d68]
00429b8d  fstp       dword ptr [esi + 0xf18]
00429b93  fld        dword ptr [esi + 0x58]
00429b96  fstp       dword ptr [esi + 0xf1c]
00429b9c  call       0x45c900

// block 00429ba1
00429ba1  pop        edi
00429ba2  xor        eax, eax
00429ba4  pop        esi
00429ba5  ret        
