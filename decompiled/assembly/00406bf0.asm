
// block 00406bf0
00406bf0  push       edi
00406bf1  mov        edi, dword ptr [0x4b43c4]
00406bf7  xor        edx, edx
00406bf9  cmp        dword ptr [edi + 0x3c], edx
00406bfc  je         0x406c03

// block 00406bfe
00406bfe  or         eax, 0xffffffff
00406c01  pop        edi
00406c02  ret        

// block 00406c03
00406c03  fldz       
00406c05  mov        ecx, 1
00406c0a  mov        dword ptr [edi + 0x3c], ecx
00406c0d  mov        eax, dword ptr [edi + 0x24]
00406c10  test       cl, al
00406c12  jne        0x406c2d

// block 00406c14
00406c14  or         eax, ecx
00406c16  fst        dword ptr [edi + 0x1c]
00406c19  mov        dword ptr [edi + 0x18], edx
00406c1c  mov        dword ptr [edi + 0x14], 0xfff0bdc1
00406c23  mov        dword ptr [edi + 0x20], 0x4b2ed0
00406c2a  mov        dword ptr [edi + 0x24], eax

// block 00406c2d
00406c2d  mov        eax, dword ptr [0x4b43cc]
00406c32  fstp       dword ptr [edi + 0x1c]
00406c35  mov        dword ptr [edi + 0x18], edx
00406c38  mov        dword ptr [edi + 0x14], 0xffffffff
00406c3f  test       byte ptr [eax + 0x7c], cl
00406c42  je         0x406c52

// block 00406c44
00406c44  cmp        dword ptr [eax + 0x28], 0x3c
00406c48  jl         0x406c52

// block 00406c4a
00406c4a  mov        dword ptr [edi + 0x51c], ecx
00406c50  jmp        0x406c58

// block 00406c52
00406c52  mov        dword ptr [edi + 0x51c], edx

// block 00406c58
00406c58  mov        eax, dword ptr [0x4b4514]
00406c5d  fld        dword ptr [eax + 0x97c]
00406c63  push       ecx
00406c64  mov        eax, 0x2e
00406c69  fstp       dword ptr [esp]
00406c6c  call       0x453e20

// block 00406c71
00406c71  mov        ecx, dword ptr [0x4b0c94]
00406c77  mov        edx, dword ptr [0x4b0c90]
00406c7d  lea        eax, [ecx + edx*2]
00406c80  cmp        eax, 5
00406c83  ja         0x406cc4

// block 00406c85
00406c85  jmp        dword ptr [eax*4 + 0x406cc8]

// block 00406c8c
00406c8c  mov        ecx, edi
00406c8e  call       0x46a5f0

// block 00406c93
00406c93  xor        eax, eax
00406c95  pop        edi
00406c96  ret        

// block 00406c97
00406c97  call       0x408120

// block 00406c9c
00406c9c  xor        eax, eax
00406c9e  pop        edi
00406c9f  ret        

// block 00406ca0
00406ca0  push       edi
00406ca1  call       0x407010

// block 00406ca6
00406ca6  xor        eax, eax
00406ca8  pop        edi
00406ca9  ret        

// block 00406caa
00406caa  push       edi
00406cab  call       0x407780

// block 00406cb0
00406cb0  xor        eax, eax
00406cb2  pop        edi
00406cb3  ret        

// block 00406cb4
00406cb4  push       edi
00406cb5  call       0x4089c0

// block 00406cba
00406cba  xor        eax, eax
00406cbc  pop        edi
00406cbd  ret        

// block 00406cbe
00406cbe  push       edi
00406cbf  call       0x408cb0

// block 00406cc4
00406cc4  xor        eax, eax
00406cc6  pop        edi
00406cc7  ret        
