
// block 00451dc0
00451dc0  mov        eax, dword ptr [0x4ce8f0]
00451dc5  fld1       
00451dc7  mov        ecx, dword ptr [eax]
00451dc9  mov        edx, dword ptr [ecx + 0xac]
00451dcf  push       0
00451dd1  push       ecx
00451dd2  fstp       dword ptr [esp]
00451dd5  push       0xff000000
00451dda  push       3
00451ddc  push       0
00451dde  push       0
00451de0  push       eax
00451de1  call       edx

// block 00451de3
00451de3  mov        eax, dword ptr [0x4ce8f0]
00451de8  mov        ecx, dword ptr [eax]
00451dea  mov        edx, dword ptr [ecx + 0x44]
00451ded  push       0
00451def  push       0
00451df1  push       0
00451df3  push       0
00451df5  push       eax
00451df6  call       edx

// block 00451df8
00451df8  test       eax, eax
00451dfa  jge        0x451e0e

// block 00451dfc
00451dfc  mov        eax, dword ptr [0x4ce8f0]
00451e01  mov        ecx, dword ptr [eax]
00451e03  mov        edx, dword ptr [ecx + 0x40]
00451e06  push       0x4ce9dc
00451e0b  push       eax
00451e0c  call       edx

// block 00451e0e
00451e0e  mov        eax, dword ptr [0x4ce8f0]
00451e13  fld1       
00451e15  mov        ecx, dword ptr [eax]
00451e17  mov        edx, dword ptr [ecx + 0xac]
00451e1d  push       0
00451e1f  push       ecx
00451e20  fstp       dword ptr [esp]
00451e23  push       0xff000000
00451e28  push       3
00451e2a  push       0
00451e2c  push       0
00451e2e  push       eax
00451e2f  call       edx

// block 00451e31
00451e31  mov        eax, dword ptr [0x4ce8f0]
00451e36  mov        ecx, dword ptr [eax]
00451e38  mov        edx, dword ptr [ecx + 0x44]
00451e3b  push       0
00451e3d  push       0
00451e3f  push       0
00451e41  push       0
00451e43  push       eax
00451e44  call       edx

// block 00451e46
00451e46  test       eax, eax
00451e48  jge        0x451e5c

// block 00451e4a
00451e4a  mov        eax, dword ptr [0x4ce8f0]
00451e4f  mov        ecx, dword ptr [eax]
00451e51  mov        edx, dword ptr [ecx + 0x40]
00451e54  push       0x4ce9dc
00451e59  push       eax
00451e5a  call       edx

// block 00451e5c
00451e5c  ret        

// block 00451e60
00451e60  push       esi
00451e61  mov        esi, dword ptr [0x4ce8cc]
00451e67  test       esi, esi
00451e69  je         0x451e70

// block 00451e6b
00451e6b  call       0x45a3c0

// block 00451e70
00451e70  fldz       
00451e72  mov        eax, dword ptr [0x4ce8f0]
00451e77  fstp       dword ptr [0x4ce9d4]
00451e7d  mov        dword ptr [0x4ce9c4], 0
00451e87  fld1       
00451e89  mov        dword ptr [0x4ce9c8], 0
00451e93  fstp       dword ptr [0x4ce9d8]
00451e99  mov        dword ptr [0x4ce9cc], 0x280
00451ea3  mov        dword ptr [0x4ce9d0], 0x1e0
00451ead  mov        ecx, dword ptr [eax]
00451eaf  mov        edx, dword ptr [ecx + 0xbc]
00451eb5  push       0x4ce9c4
00451eba  push       eax
00451ebb  call       edx

// block 00451ebd
00451ebd  pop        esi
00451ebe  jmp        0x451dc0
