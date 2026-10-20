
// block 00454b80
00454b80  push       ecx
00454b81  cmp        dword ptr [edx + 0x108], 0
00454b88  je         0x454d03

// block 00454b8e
00454b8e  cmp        dword ptr [edx + 0x124], 0
00454b95  jne        0x454d03

// block 00454b9b
00454b9b  mov        word ptr [eax + 0x3e4], cx
00454ba2  mov        edx, dword ptr [edx + 0x118]
00454ba8  lea        ecx, [ecx + ecx*8]
00454bab  lea        ecx, [edx + ecx*8]
00454bae  mov        dword ptr [eax + 0x3f4], ecx
00454bb4  fld        dword ptr [ecx + 0x24]
00454bb7  fstp       dword ptr [esp]
00454bba  mov        edx, ecx
00454bbc  fld        dword ptr [esp]
00454bbf  push       esi
00454bc0  fst        dword ptr [eax + 0x8c]
00454bc6  lea        esi, [eax + 0x2fc]
00454bcc  fstp       dword ptr [eax + 0x7c]
00454bcf  push       edi
00454bd0  fld        dword ptr [ecx + 0x2c]
00454bd3  fstp       dword ptr [esp + 8]
00454bd7  fld        dword ptr [esp + 8]
00454bdb  fst        dword ptr [eax + 0x94]
00454be1  fstp       dword ptr [eax + 0x84]
00454be7  fld        dword ptr [edx + 0x28]
00454bea  fstp       dword ptr [esp + 8]
00454bee  fld        dword ptr [esp + 8]
00454bf2  fst        dword ptr [eax + 0x88]
00454bf8  fstp       dword ptr [eax + 0x80]
00454bfe  fld        dword ptr [ecx + 0x30]
00454c01  fstp       dword ptr [esp + 8]
00454c05  fld        dword ptr [esp + 8]
00454c09  fst        dword ptr [eax + 0x98]
00454c0f  fstp       dword ptr [eax + 0x90]
00454c15  fld        dword ptr [edx + 0x38]
00454c18  fstp       dword ptr [eax + 0x58]
00454c1b  fld        dword ptr [ecx + 0x34]
00454c1e  fstp       dword ptr [eax + 0x5c]
00454c21  fldz       
00454c23  fst        dword ptr [esi + 0x38]
00454c26  fst        dword ptr [esi + 0x34]
00454c29  fst        dword ptr [esi + 0x30]
00454c2c  fst        dword ptr [esi + 0x2c]
00454c2f  fst        dword ptr [esi + 0x24]
00454c32  fst        dword ptr [esi + 0x20]
00454c35  fst        dword ptr [esi + 0x1c]
00454c38  fst        dword ptr [esi + 0x18]
00454c3b  fst        dword ptr [esi + 0x10]
00454c3e  fst        dword ptr [esi + 0xc]
00454c41  fst        dword ptr [esi + 8]
00454c44  fst        dword ptr [esi + 4]
00454c47  fld1       
00454c49  fst        dword ptr [esi + 0x3c]
00454c4c  fst        dword ptr [esi + 0x28]
00454c4f  fst        dword ptr [esi + 0x14]
00454c52  fst        dword ptr [esi]
00454c54  fst        dword ptr [eax + 0x3b8]
00454c5a  fst        dword ptr [eax + 0x3a4]
00454c60  fst        dword ptr [eax + 0x390]
00454c66  fstp       dword ptr [eax + 0x37c]
00454c6c  fst        dword ptr [eax + 0x3b4]
00454c72  fst        dword ptr [eax + 0x3b0]
00454c78  fst        dword ptr [eax + 0x3ac]
00454c7e  fst        dword ptr [eax + 0x3a8]
00454c84  fst        dword ptr [eax + 0x3a0]
00454c8a  fst        dword ptr [eax + 0x39c]
00454c90  fst        dword ptr [eax + 0x398]
00454c96  fst        dword ptr [eax + 0x394]
00454c9c  fst        dword ptr [eax + 0x38c]
00454ca2  fst        dword ptr [eax + 0x388]
00454ca8  fst        dword ptr [eax + 0x384]
00454cae  fstp       dword ptr [eax + 0x380]
00454cb4  mov        ecx, dword ptr [eax + 0x3f4]
00454cba  fld        dword ptr [eax + 0x58]
00454cbd  fld        qword ptr [0x4a3dd0]
00454cc3  fmul       st(1), st(0)
00454cc5  fxch       st(1)
00454cc7  fstp       dword ptr [esi]
00454cc9  fmul       dword ptr [eax + 0x5c]
00454ccc  fstp       dword ptr [eax + 0x310]
00454cd2  fld        dword ptr [eax + 0x58]
00454cd5  fdiv       dword ptr [ecx + 0x20]
00454cd8  fmul       dword ptr [ecx + 0x3c]
00454cdb  fstp       dword ptr [eax + 0x37c]
00454ce1  fld        dword ptr [eax + 0x5c]
00454ce4  fdiv       dword ptr [ecx + 0x1c]
00454ce7  fmul       dword ptr [ecx + 0x40]
00454cea  lea        edi, [eax + 0x33c]
00454cf0  mov        ecx, 0x10

// block 00454cf5
00454cf5  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00454cf7
00454cf7  fstp       dword ptr [eax + 0x390]
00454cfd  pop        edi
00454cfe  xor        eax, eax
00454d00  pop        esi
00454d01  pop        ecx
00454d02  ret        

// block 00454d03
00454d03  or         eax, 0xffffffff
00454d06  pop        ecx
00454d07  ret        
