
// block 00408c30
00408c30  sub        esp, 0x18
00408c33  fldz       
00408c35  mov        eax, dword ptr [0x4b43cc]
00408c3a  mov        ecx, dword ptr [eax + 0x7c]
00408c3d  fst        dword ptr [esp]
00408c40  fstp       dword ptr [esp + 8]
00408c44  push       esi
00408c45  fld        dword ptr [0x4a40f0]
00408c4b  not        ecx
00408c4d  fstp       dword ptr [esp + 0x10]
00408c51  push       edi
00408c52  fld        qword ptr [0x4a3d50]
00408c58  and        ecx, 1
00408c5b  fld        st(0)
00408c5d  push       ecx
00408c5e  fldz       
00408c60  lea        edx, [esp + 0x18]
00408c64  fsub       st(1), st(0)
00408c66  push       edx
00408c67  fxch       st(1)
00408c69  lea        eax, [esp + 0x10]
00408c6d  fstp       dword ptr [esp + 0x20]
00408c71  faddp      st(1)
00408c73  fmul       qword ptr [0x4a3cf8]
00408c79  fstp       dword ptr [esp + 0x14]
00408c7d  call       0x40cd00

// block 00408c82
00408c82  mov        eax, dword ptr [0x4b43cc]
00408c87  mov        ecx, dword ptr [eax + 0x7c]
00408c8a  not        ecx
00408c8c  and        ecx, 1
00408c8f  push       ecx
00408c90  lea        esi, [esp + 0x18]
00408c94  lea        edi, [esp + 0xc]
00408c98  call       0x4285f0

// block 00408c9d
00408c9d  pop        edi
00408c9e  xor        eax, eax
00408ca0  pop        esi
00408ca1  add        esp, 0x18
00408ca4  ret        
