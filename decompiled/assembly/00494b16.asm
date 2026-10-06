
// block 00494b16
00494b16  push       eax
00494b17  push       ebx
00494b18  push       ecx
00494b19  mov        eax, dword ptr [esp + 0x16]
00494b1d  xor        eax, 0x700
00494b22  test       eax, 0x700
00494b27  jne        0x494cb0

// block 00494b2d
00494b2d  shr        eax, 0xb
00494b30  and        eax, 0xf
00494b33  cmp        byte ptr [eax + 0x4b360c], 0
00494b3a  je         0x494cb0

// block 00494b40
00494b40  mov        eax, dword ptr [esp + 0x16]
00494b44  and        eax, 0x7fff0000
00494b49  cmp        eax, 0x7fff0000
00494b4e  je         0x494cb0

// block 00494b54
00494b54  mov        eax, dword ptr [esp + 0x2e]
00494b58  and        eax, 0x7fff0000
00494b5d  je         0x494cb0

// block 00494b63
00494b63  cmp        eax, 0x7fff0000
00494b68  je         0x494cb0

// block 00494b6e
00494b6e  mov        eax, dword ptr [esp + 0x2c]
00494b72  add        eax, eax
00494b74  jne        0x494cb0

// block 00494b7a
00494b7a  mov        eax, dword ptr [esp + 0x14]
00494b7e  add        eax, eax
00494b80  jne        0x494cb0

// block 00494b86
00494b86  mov        eax, dword ptr [esp + 0x18]
00494b8a  and        eax, 0x7fff
00494b8f  add        eax, 0x3f
00494b92  mov        ebx, dword ptr [esp + 0x30]
00494b96  and        ebx, 0x7fff
00494b9c  sub        ebx, eax
00494b9e  ja         0x494bfe

// block 00494ba0
00494ba0  mov        eax, dword ptr [esp + 0x18]
00494ba4  and        eax, 0x7fff
00494ba9  add        eax, 0xa
00494bac  mov        ebx, dword ptr [esp + 0x30]
00494bb0  and        ebx, 0x7fff
00494bb6  sub        ebx, eax
00494bb8  js         0x494cb0

// block 00494bbe
00494bbe  fld        xword ptr [esp + 0x28]
00494bc2  mov        eax, dword ptr [esp + 0x18]
00494bc6  mov        ebx, dword ptr [esp + 0x30]
00494bca  and        ebx, 0x7fff
00494bd0  mov        ecx, ebx
00494bd2  sub        ebx, eax
00494bd4  and        ebx, 7
00494bd7  or         ebx, 4
00494bda  sub        ecx, ebx
00494bdc  mov        ebx, eax
00494bde  and        ebx, 0x8000
00494be4  or         ecx, ebx
00494be6  mov        dword ptr [esp + 0x18], ecx
00494bea  fld        xword ptr [esp + 0x10]
00494bee  mov        dword ptr [esp + 0x18], eax
00494bf2  fxch       st(1)
00494bf4  fprem      
00494bf6  fstp       xword ptr [esp + 0x28]
00494bfa  fstp       st(0)
00494bfc  jmp        0x494ba0

// block 00494bfe
00494bfe  test       edx, 2
00494c04  jne        0x494c0e

// block 00494c06
00494c06  fld        xword ptr [esp + 0x10]
00494c0a  fstp       xword ptr [esp + 0x1c]

// block 00494c0e
00494c0e  fnstcw     word ptr [esp + 0x34]
00494c12  mov        eax, dword ptr [esp + 0x34]
00494c16  or         eax, 0x33f
00494c1b  mov        dword ptr [esp + 0x38], eax
00494c1f  fldcw      word ptr [esp + 0x38]
00494c23  mov        eax, dword ptr [esp + 0x18]
00494c27  and        eax, 0x7fff
00494c2c  mov        ebx, dword ptr [esp + 0x30]
00494c30  and        ebx, 0x7fff
00494c36  sub        ebx, eax
00494c38  and        ebx, 0x3f
00494c3b  or         ebx, 0x20
00494c3e  add        ebx, 1
00494c41  mov        ecx, ebx
00494c43  mov        eax, dword ptr [esp + 0x18]
00494c47  mov        ebx, dword ptr [esp + 0x30]
00494c4b  and        ebx, 0x7fff
00494c51  and        eax, 0x8000
00494c56  or         ebx, eax
00494c58  mov        dword ptr [esp + 0x18], ebx
00494c5c  fld        xword ptr [esp + 0x10]
00494c60  fabs       
00494c62  fld        xword ptr [esp + 0x28]
00494c66  fabs       

// block 00494c68
00494c68  fcom       st(1)
00494c6a  fnstsw     ax
00494c6c  and        eax, 0x100
00494c71  jne        0x494c75

// block 00494c73
00494c73  fsub       st(1)

// block 00494c75
00494c75  fxch       st(1)
00494c77  fmul       qword ptr [0x4b363c]
00494c7d  fxch       st(1)
00494c7f  sub        ecx, 1
00494c82  jne        0x494c68

// block 00494c84
00494c84  mov        ebx, dword ptr [esp + 0x30]
00494c88  fstp       xword ptr [esp + 0x28]
00494c8c  fstp       st(0)
00494c8e  fld        xword ptr [esp + 0x1c]
00494c92  fld        xword ptr [0x4b3644]
00494c98  fprem      
00494c9a  fstp       st(0)
00494c9c  fld        xword ptr [esp + 0x28]
00494ca0  fldcw      word ptr [esp + 0x34]
00494ca4  and        ebx, 0x8000
00494caa  je         0x494cba

// block 00494cac
00494cac  fchs       
00494cae  jmp        0x494cba

// block 00494cb0
00494cb0  fld        xword ptr [esp + 0x10]
00494cb4  fld        xword ptr [esp + 0x28]
00494cb8  fprem      

// block 00494cba
00494cba  test       edx, 3
00494cc0  je         0x494d18

// block 00494cc2
00494cc2  fnstsw     word ptr [esp + 0x3c]
00494cc6  test       edx, 1
00494ccc  je         0x494ced

// block 00494cce
00494cce  fnstcw     word ptr [esp + 0x34]
00494cd2  mov        eax, dword ptr [esp + 0x34]
00494cd6  or         eax, 0x300
00494cdb  mov        dword ptr [esp + 0x38], eax
00494cdf  fldcw      word ptr [esp + 0x38]
00494ce3  fmul       qword ptr [0x4b362c]
00494ce9  fldcw      word ptr [esp + 0x34]

// block 00494ced
00494ced  mov        eax, dword ptr [esp + 0x3c]
00494cf1  fxch       st(1)
00494cf3  fstp       st(0)
00494cf5  fld        xword ptr [esp + 0x1c]
00494cf9  fxch       st(1)
00494cfb  and        eax, 0x4300
00494d00  sub        esp, 0x1c
00494d03  fnstenv    [esp]
00494d06  and        dword ptr [esp + 4], 0xbcff
00494d0e  or         dword ptr [esp + 4], eax
00494d12  fldenv     [esp]
00494d15  add        esp, 0x1c

// block 00494d18
00494d18  pop        ecx
00494d19  pop        ebx
00494d1a  pop        eax
00494d1b  ret        
