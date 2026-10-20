
// block 00451f30
00451f30  sub        esp, 0x5c
00451f33  push       esi
00451f34  mov        esi, dword ptr [0x4ce8cc]
00451f3a  call       0x45a3c0

// block 00451f3f
00451f3f  fld        dword ptr [edi]
00451f41  fstp       dword ptr [esp + 4]
00451f45  mov        eax, dword ptr [esp + 4]
00451f49  fld        dword ptr [edi + 4]
00451f4c  mov        dword ptr [esp + 0x10], eax
00451f50  fstp       dword ptr [esp + 8]
00451f54  mov        ecx, dword ptr [esp + 8]
00451f58  fldz       
00451f5a  mov        dword ptr [esp + 0x14], ecx
00451f5e  fst        dword ptr [esp + 0xc]
00451f62  push       2
00451f64  fld        dword ptr [edi + 8]
00451f67  mov        edx, dword ptr [esp + 0x10]
00451f6b  fstp       dword ptr [esp + 8]
00451f6f  mov        dword ptr [esp + 0x1c], edx
00451f73  fld        dword ptr [edi + 4]
00451f76  mov        eax, dword ptr [esp + 8]
00451f7a  fstp       dword ptr [esp + 0xc]
00451f7e  mov        dword ptr [esp + 0x28], eax
00451f82  mov        ecx, dword ptr [esp + 0xc]
00451f86  mov        dword ptr [esp + 0x2c], ecx
00451f8a  fst        dword ptr [esp + 0x10]
00451f8e  mov        edx, dword ptr [esp + 0x10]
00451f92  fld        dword ptr [edi]
00451f94  mov        dword ptr [esp + 0x30], edx
00451f98  fstp       dword ptr [esp + 8]
00451f9c  mov        eax, dword ptr [esp + 8]
00451fa0  fld        dword ptr [edi + 0xc]
00451fa3  mov        dword ptr [esp + 0x3c], eax
00451fa7  fstp       dword ptr [esp + 0xc]
00451fab  mov        ecx, dword ptr [esp + 0xc]
00451faf  mov        dword ptr [esp + 0x40], ecx
00451fb3  fst        dword ptr [esp + 0x10]
00451fb7  mov        edx, dword ptr [esp + 0x10]
00451fbb  fld        dword ptr [edi + 8]
00451fbe  mov        dword ptr [esp + 0x44], edx
00451fc2  fstp       dword ptr [esp + 8]
00451fc6  mov        eax, dword ptr [esp + 8]
00451fca  fld        dword ptr [edi + 0xc]
00451fcd  mov        dword ptr [esp + 0x50], eax
00451fd1  mov        eax, dword ptr [0x4ce8f0]
00451fd6  fstp       dword ptr [esp + 0xc]
00451fda  mov        ecx, dword ptr [esp + 0xc]
00451fde  fstp       dword ptr [esp + 0x10]
00451fe2  mov        edx, dword ptr [esp + 0x10]
00451fe6  fld1       
00451fe8  push       4
00451fea  fst        dword ptr [esp + 0x60]
00451fee  mov        dword ptr [esp + 0x5c], edx
00451ff2  fst        dword ptr [esp + 0x4c]
00451ff6  mov        dword ptr [esp + 0x58], ecx
00451ffa  fst        dword ptr [esp + 0x38]
00451ffe  mov        dword ptr [esp + 0x64], ebx
00452002  fstp       dword ptr [esp + 0x24]
00452006  mov        dword ptr [esp + 0x50], ebx
0045200a  mov        dword ptr [esp + 0x3c], ebx
0045200e  mov        dword ptr [esp + 0x28], ebx
00452012  mov        ecx, dword ptr [eax]
00452014  mov        edx, dword ptr [ecx + 0x10c]
0045201a  xor        esi, esi
0045201c  push       esi
0045201d  push       eax
0045201e  call       edx

// block 00452020
00452020  mov        eax, dword ptr [0x4ce8f0]
00452025  mov        ecx, dword ptr [eax]
00452027  mov        edx, dword ptr [ecx + 0x10c]
0045202d  push       2
0045202f  push       1
00452031  push       esi
00452032  push       eax
00452033  call       edx

// block 00452035
00452035  mov        eax, dword ptr [0x4ce8f0]
0045203a  mov        ecx, dword ptr [eax]
0045203c  mov        edx, dword ptr [ecx + 0x10c]
00452042  push       esi
00452043  push       5
00452045  push       esi
00452046  push       eax
00452047  call       edx

// block 00452049
00452049  mov        eax, dword ptr [0x4ce8f0]
0045204e  mov        ecx, dword ptr [eax]
00452050  mov        edx, dword ptr [ecx + 0x10c]
00452056  push       esi
00452057  push       2
00452059  push       esi
0045205a  push       eax
0045205b  call       edx

// block 0045205d
0045205d  mov        eax, dword ptr [0x4ce8f0]
00452062  mov        ecx, dword ptr [eax]
00452064  mov        edx, dword ptr [ecx + 0xe4]
0045206a  push       6
0045206c  push       0x14
0045206e  push       eax
0045206f  call       edx

// block 00452071
00452071  mov        eax, dword ptr [0x4ce8f0]
00452076  mov        ecx, dword ptr [eax]
00452078  mov        edx, dword ptr [ecx + 0x164]
0045207e  push       0x44
00452080  push       eax
00452081  call       edx

// block 00452083
00452083  mov        eax, dword ptr [0x4ce8f0]
00452088  mov        ecx, dword ptr [eax]
0045208a  push       0x14
0045208c  lea        edx, [esp + 0x14]
00452090  push       edx
00452091  push       2
00452093  push       5
00452095  push       eax
00452096  mov        eax, dword ptr [ecx + 0x14c]
0045209c  call       eax

// block 0045209e
0045209e  mov        eax, dword ptr [0x4ce8cc]
004520a3  mov        cl, 0xff
004520a5  push       4
004520a7  mov        byte ptr [eax + 0x4b5642], cl
004520ad  mov        dword ptr [eax + 0x4b5648], esi
004520b3  mov        dword ptr [eax + 0x4b563c], esi
004520b9  mov        byte ptr [eax + 0x4b5641], cl
004520bf  mov        byte ptr [eax + 0x4b5640], 7
004520c6  mov        byte ptr [eax + 0x4b5643], cl
004520cc  mov        eax, dword ptr [0x4ce8f0]
004520d1  mov        ecx, dword ptr [eax]
004520d3  mov        edx, dword ptr [ecx + 0x10c]
004520d9  push       4
004520db  push       esi
004520dc  push       eax
004520dd  call       edx

// block 004520df
004520df  mov        eax, dword ptr [0x4ce8f0]
004520e4  mov        ecx, dword ptr [eax]
004520e6  mov        edx, dword ptr [ecx + 0x10c]
004520ec  push       4
004520ee  push       1
004520f0  push       esi
004520f1  push       eax
004520f2  call       edx

// block 004520f4
004520f4  mov        eax, dword ptr [0x4ce8f0]
004520f9  mov        ecx, dword ptr [eax]
004520fb  mov        edx, dword ptr [ecx + 0x10c]
00452101  push       2
00452103  push       5
00452105  push       esi
00452106  push       eax
00452107  call       edx

// block 00452109
00452109  mov        eax, dword ptr [0x4ce8f0]
0045210e  mov        ecx, dword ptr [eax]
00452110  mov        edx, dword ptr [ecx + 0x10c]
00452116  push       2
00452118  push       2
0045211a  push       esi
0045211b  push       eax
0045211c  call       edx

// block 0045211e
0045211e  pop        esi
0045211f  add        esp, 0x5c
00452122  ret        
