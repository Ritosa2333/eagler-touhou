
// block 0042b100
0042b100  push       esi
0042b101  mov        esi, ecx
0042b103  fld        dword ptr [esi + 0x50]
0042b106  push       edi
0042b107  fadd       qword ptr [0x4a3d70]
0042b10d  sub        esp, 8
0042b110  lea        edi, [esi + 0x660]
0042b116  fadd       qword ptr [0x4a3d28]
0042b11c  fstp       dword ptr [esi + 0xa84]
0042b122  fld        dword ptr [esi + 0x54]
0042b125  fadd       qword ptr [0x4a3d68]
0042b12b  fstp       dword ptr [esi + 0xa88]
0042b131  fld        dword ptr [esi + 0x58]
0042b134  fstp       dword ptr [esi + 0xa8c]
0042b13a  fld        dword ptr [0x4a3e08]
0042b140  fstp       dword ptr [esp + 4]
0042b144  fld        dword ptr [esi + 0x68]
0042b147  fstp       dword ptr [esp]
0042b14a  call       0x464640

// block 0042b14f
0042b14f  mov        eax, dword ptr [0x4ce8cc]
0042b154  fstp       dword ptr [edi + 0x2c]
0042b157  or         dword ptr [edi + 0x47c], 4
0042b15e  push       eax
0042b15f  mov        eax, edi
0042b161  call       0x45c900

// block 0042b166
0042b166  fldz       
0042b168  fcomp      dword ptr [esi + 0x78]
0042b16b  fnstsw     ax
0042b16d  test       ah, 0x44
0042b170  jp         0x42b1b1

// block 0042b172
0042b172  fld        dword ptr [esi + 0x50]
0042b175  mov        ecx, dword ptr [0x4ce8cc]
0042b17b  fadd       qword ptr [0x4a3d70]
0042b181  lea        eax, [esi + 0xb14]
0042b187  push       ecx
0042b188  fadd       qword ptr [0x4a3d28]
0042b18e  fstp       dword ptr [esi + 0xf38]
0042b194  fld        dword ptr [esi + 0x54]
0042b197  fadd       qword ptr [0x4a3d68]
0042b19d  fstp       dword ptr [esi + 0xf3c]
0042b1a3  fld        dword ptr [esi + 0x58]
0042b1a6  fstp       dword ptr [esi + 0xf40]
0042b1ac  call       0x45c900

// block 0042b1b1
0042b1b1  pop        edi
0042b1b2  xor        eax, eax
0042b1b4  pop        esi
0042b1b5  ret        
