
// block 00463e80
00463e80  push       ecx
00463e81  push       ebp
00463e82  mov        ebp, 0x8000
00463e87  push       esi
00463e88  test       dword ptr [0x4cee78], ebp
00463e8e  je         0x463ea1

// block 00463e90
00463e90  push       0x4cf128
00463e95  call       dword ptr [0x498088]

// block 00463e9b
00463e9b  inc        byte ptr [0x4cf21a]

// block 00463ea1
00463ea1  push       0
00463ea3  push       0x80
00463ea8  push       2
00463eaa  push       0
00463eac  push       1
00463eae  push       0x40000000
00463eb3  push       edi
00463eb4  call       dword ptr [0x49809c]

// block 00463eba
00463eba  mov        esi, eax
00463ebc  push       0
00463ebe  cmp        esi, -1
00463ec1  jne        0x463f23

// block 00463ec3
00463ec3  push       0
00463ec5  lea        eax, [esp + 0x18]
00463ec9  push       eax
00463eca  push       0x400
00463ecf  call       dword ptr [0x4980e4]

// block 00463ed5
00463ed5  push       eax
00463ed6  push       0
00463ed8  push       0x1300
00463edd  call       dword ptr [0x4980fc]

// block 00463ee3
00463ee3  mov        ecx, dword ptr [esp + 0x10]
00463ee7  push       ecx
00463ee8  push       edi
00463ee9  push       0x4a3680
00463eee  call       0x4654b0

// block 00463ef3
00463ef3  mov        edx, dword ptr [esp + 0x1c]
00463ef7  add        esp, 0xc
00463efa  push       edx
00463efb  call       dword ptr [0x498100]

// block 00463f01
00463f01  test       dword ptr [0x4cee78], ebp
00463f07  je         0x463f1a

// block 00463f09
00463f09  push       0x4cf128
00463f0e  call       dword ptr [0x49808c]

// block 00463f14
00463f14  dec        byte ptr [0x4cf21a]

// block 00463f1a
00463f1a  pop        esi
00463f1b  or         eax, 0xffffffff
00463f1e  pop        ebp
00463f1f  pop        ecx
00463f20  ret        4

// block 00463f23
00463f23  mov        ecx, dword ptr [esp + 0x14]
00463f27  lea        eax, [esp + 0xc]
00463f2b  push       eax
00463f2c  push       ebx
00463f2d  push       ecx
00463f2e  push       esi
00463f2f  call       dword ptr [0x4980ac]

// block 00463f35
00463f35  push       esi
00463f36  cmp        ebx, dword ptr [esp + 0xc]
00463f3a  je         0x463f74

// block 00463f3c
00463f3c  call       dword ptr [0x4980a4]

// block 00463f42
00463f42  push       edi
00463f43  push       0x4a369c
00463f48  call       0x4654b0

// block 00463f4d
00463f4d  add        esp, 8
00463f50  test       dword ptr [0x4cee78], ebp
00463f56  je         0x463f69

// block 00463f58
00463f58  push       0x4cf128
00463f5d  call       dword ptr [0x49808c]

// block 00463f63
00463f63  dec        byte ptr [0x4cf21a]

// block 00463f69
00463f69  pop        esi
00463f6a  mov        eax, 0xfffffffe
00463f6f  pop        ebp
00463f70  pop        ecx
00463f71  ret        4

// block 00463f74
00463f74  call       dword ptr [0x4980a4]

// block 00463f7a
00463f7a  push       edi
00463f7b  push       0x4a36b8
00463f80  call       0x4654b0

// block 00463f85
00463f85  add        esp, 8
00463f88  test       dword ptr [0x4cee78], ebp
00463f8e  je         0x463fa1

// block 00463f90
00463f90  push       0x4cf128
00463f95  call       dword ptr [0x49808c]

// block 00463f9b
00463f9b  dec        byte ptr [0x4cf21a]

// block 00463fa1
00463fa1  pop        esi
00463fa2  xor        eax, eax
00463fa4  pop        ebp
00463fa5  pop        ecx
00463fa6  ret        4
