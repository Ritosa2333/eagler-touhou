
// block 00476da8
00476da8  mov        edi, edi
00476daa  push       ebp
00476dab  mov        ebp, esp
00476dad  sub        esp, 0xc
00476db0  push       esi
00476db1  lea        eax, [ebp - 4]
00476db4  xor        esi, esi
00476db6  push       eax
00476db7  mov        dword ptr [ebp - 4], esi
00476dba  call       0x4772b2

// block 00476dbf
00476dbf  pop        ecx
00476dc0  test       eax, eax
00476dc2  je         0x476dd1

// block 00476dc4
00476dc4  push       esi
00476dc5  push       esi
00476dc6  push       esi
00476dc7  push       esi
00476dc8  push       esi
00476dc9  call       0x470dc6

// block 00476dce
00476dce  add        esp, 0x14

// block 00476dd1
00476dd1  cmp        dword ptr [ebp - 4], esi
00476dd4  jne        0x476ddd

// block 00476dd6
00476dd6  xor        eax, eax
00476dd8  jmp        0x476f35

// block 00476ddd
00476ddd  mov        edx, dword ptr [edi + 0x14]
00476de0  push       ebx
00476de1  xor        ebx, ebx
00476de3  inc        ebx
00476de4  cmp        edx, dword ptr [0x4adae0]
00476dea  jne        0x476df8

// block 00476dec
00476dec  cmp        edx, dword ptr [0x4adaec]
00476df2  je         0x476f10

// block 00476df8
00476df8  cmp        dword ptr [0x4b41c4], esi
00476dfe  je         0x476ebc

// block 00476e04
00476e04  movzx      eax, word ptr [0x4b41be]
00476e0b  movzx      ecx, word ptr [0x4b41b8]
00476e12  push       eax
00476e13  movzx      eax, word ptr [0x4b41bc]
00476e1a  push       eax
00476e1b  movzx      eax, word ptr [0x4b41ba]
00476e22  push       eax
00476e23  cmp        word ptr [0x4b41b0], si
00476e2a  jne        0x476e41

// block 00476e2c
00476e2c  movzx      eax, word ptr [0x4b41b4]
00476e33  push       esi
00476e34  push       eax
00476e35  movzx      eax, word ptr [0x4b41b6]
00476e3c  push       eax
00476e3d  push       edx
00476e3e  push       ebx
00476e3f  jmp        0x476e4d

// block 00476e41
00476e41  movzx      eax, word ptr [0x4b41b6]
00476e48  push       eax
00476e49  push       esi
00476e4a  push       esi
00476e4b  push       edx
00476e4c  push       esi

// block 00476e4d
00476e4d  movzx      eax, word ptr [0x4b41b2]
00476e54  push       ebx
00476e55  call       0x476bb1

// block 00476e5a
00476e5a  movzx      eax, word ptr [0x4b416a]
00476e61  movzx      ecx, word ptr [0x4b4164]
00476e68  add        esp, 0x24
00476e6b  push       eax
00476e6c  movzx      eax, word ptr [0x4b4168]
00476e73  push       eax
00476e74  movzx      eax, word ptr [0x4b4166]
00476e7b  push       eax
00476e7c  cmp        word ptr [0x4b415c], si
00476e83  jne        0x476e9c

// block 00476e85
00476e85  movzx      eax, word ptr [0x4b4160]
00476e8c  push       esi
00476e8d  push       eax
00476e8e  movzx      eax, word ptr [0x4b4162]
00476e95  push       eax
00476e96  push       dword ptr [edi + 0x14]
00476e99  push       ebx
00476e9a  jmp        0x476eaa

// block 00476e9c
00476e9c  movzx      eax, word ptr [0x4b4162]
00476ea3  push       eax
00476ea4  push       esi
00476ea5  push       esi
00476ea6  push       dword ptr [edi + 0x14]
00476ea9  push       esi

// block 00476eaa
00476eaa  movzx      eax, word ptr [0x4b415e]
00476eb1  push       esi
00476eb2  call       0x476bb1

// block 00476eb7
00476eb7  add        esp, 0x24
00476eba  jmp        0x476f10

// block 00476ebc
00476ebc  cmp        edx, 0x6b
00476ebf  push       3
00476ec1  pop        eax
00476ec2  push       2
00476ec4  pop        ecx
00476ec5  mov        dword ptr [ebp - 0xc], 0xb
00476ecc  mov        dword ptr [ebp - 8], ebx
00476ecf  jge        0x476ee4

// block 00476ed1
00476ed1  push       4
00476ed3  pop        eax
00476ed4  mov        ecx, ebx
00476ed6  mov        dword ptr [ebp - 0xc], 0xa
00476edd  mov        dword ptr [ebp - 8], 5

// block 00476ee4
00476ee4  push       esi
00476ee5  push       esi
00476ee6  push       esi
00476ee7  push       esi
00476ee8  push       esi
00476ee9  push       ecx
00476eea  push       edx
00476eeb  push       ebx
00476eec  push       ebx
00476eed  push       2
00476eef  pop        ecx
00476ef0  call       0x476bb1

// block 00476ef5
00476ef5  mov        eax, dword ptr [ebp - 0xc]
00476ef8  push       esi
00476ef9  push       esi
00476efa  push       esi
00476efb  push       esi
00476efc  push       esi
00476efd  push       dword ptr [ebp - 8]
00476f00  push       dword ptr [edi + 0x14]
00476f03  push       ebx
00476f04  push       esi
00476f05  push       2
00476f07  pop        ecx
00476f08  call       0x476bb1

// block 00476f0d
00476f0d  add        esp, 0x48

// block 00476f10
00476f10  mov        ecx, dword ptr [0x4adae4]
00476f16  mov        eax, dword ptr [0x4adaf0]
00476f1b  cmp        ecx, eax
00476f1d  mov        edx, dword ptr [edi + 0x1c]
00476f20  jge        0x476f38

// block 00476f22
00476f22  cmp        edx, ecx
00476f24  jl         0x476f48

// block 00476f26
00476f26  cmp        edx, eax
00476f28  jg         0x476f48

// block 00476f2a
00476f2a  cmp        edx, ecx
00476f2c  jle        0x476f4c

// block 00476f2e
00476f2e  cmp        edx, eax
00476f30  jge        0x476f4c

// block 00476f32
00476f32  mov        eax, ebx

// block 00476f34
00476f34  pop        ebx

// block 00476f35
00476f35  pop        esi
00476f36  leave      
00476f37  ret        

// block 00476f38
00476f38  cmp        edx, eax
00476f3a  jl         0x476f32

// block 00476f3c
00476f3c  cmp        edx, ecx
00476f3e  jg         0x476f32

// block 00476f40
00476f40  cmp        edx, eax
00476f42  jle        0x476f4c

// block 00476f44
00476f44  cmp        edx, ecx
00476f46  jge        0x476f4c

// block 00476f48
00476f48  xor        eax, eax
00476f4a  jmp        0x476f34

// block 00476f4c
00476f4c  mov        eax, dword ptr [edi + 8]
00476f4f  imul       eax, eax, 0x3c
00476f52  add        eax, dword ptr [edi + 4]
00476f55  imul       eax, eax, 0x3c
00476f58  add        eax, dword ptr [edi]
00476f5a  imul       eax, eax, 0x3e8
00476f60  cmp        edx, ecx
00476f62  jne        0x476f71

// block 00476f64
00476f64  xor        ecx, ecx
00476f66  cmp        eax, dword ptr [0x4adae8]
00476f6c  setge      cl
00476f6f  jmp        0x476f7c

// block 00476f71
00476f71  xor        ecx, ecx
00476f73  cmp        eax, dword ptr [0x4adaf4]
00476f79  setl       cl

// block 00476f7c
00476f7c  mov        eax, ecx
00476f7e  jmp        0x476f34
