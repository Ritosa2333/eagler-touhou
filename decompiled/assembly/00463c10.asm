
// block 00463c10
00463c10  push       ecx
00463c11  push       ebx
00463c12  push       ebp
00463c13  push       esi
00463c14  mov        ebp, 0x8000
00463c19  push       edi
00463c1a  mov        edi, eax
00463c1c  test       dword ptr [0x4cee78], ebp
00463c22  je         0x463c35

// block 00463c24
00463c24  push       0x4cf128
00463c29  call       dword ptr [0x498088]

// block 00463c2f
00463c2f  inc        byte ptr [0x4cf21a]

// block 00463c35
00463c35  cmp        dword ptr [esp + 0x1c], 0
00463c3a  jne        0x463cfd

// block 00463c40
00463c40  push       0x5c
00463c42  push       edi
00463c43  call       0x46d260

// block 00463c48
00463c48  add        esp, 8
00463c4b  test       eax, eax
00463c4d  jne        0x463c53

// block 00463c4f
00463c4f  mov        eax, edi
00463c51  jmp        0x463c54

// block 00463c53
00463c53  inc        eax

// block 00463c54
00463c54  push       0x2f
00463c56  push       eax
00463c57  call       0x46d260

// block 00463c5c
00463c5c  mov        ebp, eax
00463c5e  add        esp, 8
00463c61  test       ebp, ebp
00463c63  jne        0x463c69

// block 00463c65
00463c65  mov        ebp, edi
00463c67  jmp        0x463c6a

// block 00463c69
00463c69  inc        ebp

// block 00463c6a
00463c6a  mov        edi, dword ptr [0x4d4c90]
00463c70  test       edi, edi
00463c72  je         0x463c98

// block 00463c74
00463c74  mov        esi, dword ptr [0x4d4c94]
00463c7a  test       esi, esi
00463c7c  jle        0x463c98

// block 00463c7e
00463c7e  mov        edi, edi

// block 00463c80
00463c80  mov        eax, dword ptr [edi]
00463c82  push       eax
00463c83  push       ebp
00463c84  call       0x46e3f8

// block 00463c89
00463c89  add        esp, 8
00463c8c  test       eax, eax
00463c8e  je         0x463cf8

// block 00463c90
00463c90  dec        esi
00463c91  add        edi, 0x10
00463c94  test       esi, esi
00463c96  jg         0x463c80

// block 00463c98
00463c98  xor        edi, edi

// block 00463c9a
00463c9a  mov        eax, dword ptr [esp + 0x18]
00463c9e  mov        dword ptr [esp + 0x10], edi
00463ca2  test       eax, eax
00463ca4  je         0x463ca8

// block 00463ca6
00463ca6  mov        dword ptr [eax], edi

// block 00463ca8
00463ca8  test       edi, edi
00463caa  je         0x463d6e

// block 00463cb0
00463cb0  push       ebp
00463cb1  push       0x4a3620
00463cb6  call       0x4654b0

// block 00463cbb
00463cbb  mov        ecx, dword ptr [esp + 0x18]
00463cbf  push       ecx
00463cc0  call       0x46d04a

// block 00463cc5
00463cc5  mov        ebx, eax
00463cc7  add        esp, 0xc
00463cca  test       ebx, ebx
00463ccc  je         0x463d6e

// block 00463cd2
00463cd2  push       ebx
00463cd3  push       0x4d4c90
00463cd8  mov        ecx, ebp
00463cda  call       0x44b7b0

// block 00463cdf
00463cdf  mov        esi, 2
00463ce4  mov        edi, 0x4ce8e8
00463ce9  call       0x431800

// block 00463cee
00463cee  pop        edi
00463cef  pop        esi
00463cf0  pop        ebp
00463cf1  mov        eax, ebx
00463cf3  pop        ebx
00463cf4  pop        ecx
00463cf5  ret        8

// block 00463cf8
00463cf8  mov        edi, dword ptr [edi + 8]
00463cfb  jmp        0x463c9a

// block 00463cfd
00463cfd  push       edi
00463cfe  push       0x4a3634
00463d03  call       0x4654b0

// block 00463d08
00463d08  add        esp, 8
00463d0b  push       0
00463d0d  push       0x8000080
00463d12  push       3
00463d14  push       0
00463d16  push       1
00463d18  push       0x80000000
00463d1d  push       edi
00463d1e  call       dword ptr [0x49809c]

// block 00463d24
00463d24  mov        esi, eax
00463d26  cmp        esi, -1
00463d29  jne        0x463d3b

// block 00463d2b
00463d2b  push       edi
00463d2c  push       0x4a3644
00463d31  call       0x4654b0

// block 00463d36
00463d36  add        esp, 8
00463d39  jmp        0x463d73

// block 00463d3b
00463d3b  push       0
00463d3d  push       esi
00463d3e  call       dword ptr [0x4980b0]

// block 00463d44
00463d44  push       eax
00463d45  mov        dword ptr [esp + 0x14], eax
00463d49  call       0x46d04a

// block 00463d4e
00463d4e  mov        ebx, eax
00463d50  add        esp, 4
00463d53  test       ebx, ebx
00463d55  jne        0x463d96

// block 00463d57
00463d57  push       edi
00463d58  push       0x4a3660
00463d5d  call       0x4654b0

// block 00463d62
00463d62  add        esp, 8
00463d65  push       esi
00463d66  call       dword ptr [0x4980a4]

// block 00463d6c
00463d6c  jmp        0x463d73

// block 00463d6e
00463d6e  mov        ebp, 0x8000

// block 00463d73
00463d73  test       dword ptr [0x4cee78], ebp
00463d79  je         0x463d8c

// block 00463d7b
00463d7b  push       0x4cf128
00463d80  call       dword ptr [0x49808c]

// block 00463d86
00463d86  dec        byte ptr [0x4cf21a]

// block 00463d8c
00463d8c  pop        edi
00463d8d  pop        esi
00463d8e  pop        ebp
00463d8f  xor        eax, eax
00463d91  pop        ebx
00463d92  pop        ecx
00463d93  ret        8

// block 00463d96
00463d96  mov        eax, dword ptr [esp + 0x10]
00463d9a  push       0
00463d9c  lea        edx, [esp + 0x14]
00463da0  push       edx
00463da1  push       eax
00463da2  push       ebx
00463da3  push       esi
00463da4  call       dword ptr [0x4980a8]

// block 00463daa
00463daa  mov        eax, dword ptr [esp + 0x18]
00463dae  test       eax, eax
00463db0  je         0x463db8

// block 00463db2
00463db2  mov        ecx, dword ptr [esp + 0x10]
00463db6  mov        dword ptr [eax], ecx

// block 00463db8
00463db8  push       esi
00463db9  call       dword ptr [0x4980a4]

// block 00463dbf
00463dbf  test       dword ptr [0x4cee78], ebp
00463dc5  je         0x463dd8

// block 00463dc7
00463dc7  push       0x4cf128
00463dcc  call       dword ptr [0x49808c]

// block 00463dd2
00463dd2  dec        byte ptr [0x4cf21a]

// block 00463dd8
00463dd8  pop        edi
00463dd9  pop        esi
00463dda  pop        ebp
00463ddb  mov        eax, ebx
00463ddd  pop        ebx
00463dde  pop        ecx
00463ddf  ret        8
