
// block 00490d7b
00490d7b  mov        edi, edi
00490d7d  push       ebp
00490d7e  mov        ebp, esp
00490d80  sub        esp, 0x14
00490d83  mov        eax, dword ptr [ebp + 8]
00490d86  push       ebx
00490d87  xor        ebx, ebx
00490d89  mov        dword ptr [ebp - 0xc], ebx
00490d8c  cmp        eax, ebx
00490d8e  jne        0x490dad

// block 00490d90
00490d90  call       0x46e96f

// block 00490d95
00490d95  push       ebx
00490d96  push       ebx
00490d97  push       ebx
00490d98  push       ebx
00490d99  push       ebx
00490d9a  mov        dword ptr [eax], 0x16
00490da0  call       0x470f2d

// block 00490da5
00490da5  add        esp, 0x14
00490da8  or         eax, 0xffffffff
00490dab  jmp        0x490e1b

// block 00490dad
00490dad  push       esi
00490dae  mov        esi, dword ptr [eax]
00490db0  push       edi
00490db1  mov        dword ptr [ebp - 4], esi
00490db4  cmp        esi, ebx
00490db6  je         0x490e0b

// block 00490db8
00490db8  push       0x3d
00490dba  push       esi
00490dbb  call       0x49143d

// block 00490dc0
00490dc0  mov        edi, eax
00490dc2  pop        ecx
00490dc3  pop        ecx
00490dc4  mov        dword ptr [ebp - 0x14], edi
00490dc7  cmp        edi, ebx
00490dc9  je         0x490e0b

// block 00490dcb
00490dcb  cmp        esi, edi
00490dcd  je         0x490e0b

// block 00490dcf
00490dcf  xor        eax, eax
00490dd1  cmp        byte ptr [edi + 1], bl
00490dd4  sete       al
00490dd7  mov        dword ptr [ebp - 8], eax
00490dda  mov        eax, dword ptr [0x4b3d80]
00490ddf  cmp        eax, dword ptr [0x4b3d84]
00490de5  jne        0x490df1

// block 00490de7
00490de7  call       0x490d21

// block 00490dec
00490dec  mov        dword ptr [0x4b3d80], eax

// block 00490df1
00490df1  cmp        eax, ebx
00490df3  jne        0x490e55

// block 00490df5
00490df5  cmp        dword ptr [ebp + 0xc], ebx
00490df8  je         0x490e1e

// block 00490dfa
00490dfa  cmp        dword ptr [0x4b3d88], ebx
00490e00  je         0x490e1e

// block 00490e02
00490e02  call       0x48e343

// block 00490e07
00490e07  test       eax, eax
00490e09  je         0x490e55

// block 00490e0b
00490e0b  call       0x46e96f

// block 00490e10
00490e10  mov        dword ptr [eax], 0x16

// block 00490e16
00490e16  or         eax, 0xffffffff

// block 00490e19
00490e19  pop        edi
00490e1a  pop        esi

// block 00490e1b
00490e1b  pop        ebx
00490e1c  leave      
00490e1d  ret        

// block 00490e1e
00490e1e  cmp        dword ptr [ebp - 8], ebx
00490e21  jne        0x490fbf

// block 00490e27
00490e27  push       4
00490e29  call       0x4777ce

// block 00490e2e
00490e2e  pop        ecx
00490e2f  mov        dword ptr [0x4b3d80], eax
00490e34  cmp        eax, ebx
00490e36  je         0x490e16

// block 00490e38
00490e38  mov        dword ptr [eax], ebx
00490e3a  cmp        dword ptr [0x4b3d88], ebx
00490e40  jne        0x490e55

// block 00490e42
00490e42  push       4
00490e44  call       0x4777ce

// block 00490e49
00490e49  pop        ecx
00490e4a  mov        dword ptr [0x4b3d88], eax
00490e4f  cmp        eax, ebx
00490e51  je         0x490e16

// block 00490e53
00490e53  mov        dword ptr [eax], ebx

// block 00490e55
00490e55  mov        esi, dword ptr [0x4b3d80]
00490e5b  mov        dword ptr [ebp - 0x10], esi
00490e5e  cmp        esi, ebx
00490e60  je         0x490e16

// block 00490e62
00490e62  sub        edi, dword ptr [ebp - 4]
00490e65  push       dword ptr [ebp - 4]
00490e68  call       0x490ccf

// block 00490e6d
00490e6d  mov        edi, eax
00490e6f  cmp        edi, ebx
00490e71  pop        ecx
00490e72  jl         0x490ec6

// block 00490e74
00490e74  cmp        dword ptr [esi], ebx
00490e76  je         0x490ec6

// block 00490e78
00490e78  lea        esi, [esi + edi*4]
00490e7b  push       dword ptr [esi]
00490e7d  call       0x46c941

// block 00490e82
00490e82  pop        ecx
00490e83  cmp        dword ptr [ebp - 8], ebx
00490e86  jne        0x490ea3

// block 00490e88
00490e88  mov        eax, dword ptr [ebp - 4]
00490e8b  mov        dword ptr [esi], eax
00490e8d  mov        eax, dword ptr [ebp + 8]
00490e90  mov        dword ptr [eax], ebx
00490e92  jmp        0x490f19

// block 00490e97
00490e97  mov        eax, dword ptr [esi + 4]
00490e9a  mov        dword ptr [esi], eax
00490e9c  mov        eax, dword ptr [ebp - 0x10]
00490e9f  inc        edi
00490ea0  lea        esi, [eax + edi*4]

// block 00490ea3
00490ea3  cmp        dword ptr [esi], ebx
00490ea5  jne        0x490e97

// block 00490ea7
00490ea7  cmp        edi, 0x3fffffff
00490ead  jae        0x490f19

// block 00490eaf
00490eaf  push       4
00490eb1  push       edi
00490eb2  push       dword ptr [0x4b3d80]
00490eb8  call       0x4778ad

// block 00490ebd
00490ebd  add        esp, 0xc
00490ec0  cmp        eax, ebx
00490ec2  je         0x490f19

// block 00490ec4
00490ec4  jmp        0x490f14

// block 00490ec6
00490ec6  cmp        dword ptr [ebp - 8], ebx
00490ec9  jne        0x490fb1

// block 00490ecf
00490ecf  cmp        edi, ebx
00490ed1  jge        0x490ed5

// block 00490ed3
00490ed3  neg        edi

// block 00490ed5
00490ed5  lea        eax, [edi + 2]
00490ed8  cmp        eax, edi
00490eda  jl         0x490e16

// block 00490ee0
00490ee0  cmp        eax, 0x3fffffff
00490ee5  jae        0x490e16

// block 00490eeb
00490eeb  push       eax
00490eec  push       4
00490eee  push       dword ptr [0x4b3d80]
00490ef4  call       0x4778ad

// block 00490ef9
00490ef9  add        esp, 0xc
00490efc  cmp        eax, ebx
00490efe  je         0x490e16

// block 00490f04
00490f04  mov        edx, dword ptr [ebp - 4]
00490f07  lea        ecx, [eax + edi*4]
00490f0a  mov        dword ptr [ecx], edx
00490f0c  mov        dword ptr [ecx + 4], ebx
00490f0f  mov        ecx, dword ptr [ebp + 8]
00490f12  mov        dword ptr [ecx], ebx

// block 00490f14
00490f14  mov        dword ptr [0x4b3d80], eax

// block 00490f19
00490f19  cmp        dword ptr [ebp + 0xc], ebx
00490f1c  je         0x490f96

// block 00490f1e
00490f1e  mov        esi, dword ptr [ebp - 4]
00490f21  push       1
00490f23  push       esi
00490f24  call       0x475860

// block 00490f29
00490f29  inc        eax
00490f2a  pop        ecx
00490f2b  inc        eax
00490f2c  push       eax
00490f2d  call       0x477813

// block 00490f32
00490f32  mov        edi, eax
00490f34  pop        ecx
00490f35  pop        ecx
00490f36  cmp        edi, ebx
00490f38  je         0x490f96

// block 00490f3a
00490f3a  push       esi
00490f3b  push       esi
00490f3c  call       0x475860

// block 00490f41
00490f41  inc        eax
00490f42  pop        ecx
00490f43  inc        eax
00490f44  push       eax
00490f45  push       edi
00490f46  call       0x47a2b9

// block 00490f4b
00490f4b  add        esp, 0xc
00490f4e  test       eax, eax
00490f50  je         0x490f5f

// block 00490f52
00490f52  push       ebx
00490f53  push       ebx
00490f54  push       ebx
00490f55  push       ebx
00490f56  push       ebx
00490f57  call       0x470dc6

// block 00490f5c
00490f5c  add        esp, 0x14

// block 00490f5f
00490f5f  mov        ecx, dword ptr [ebp - 8]
00490f62  mov        eax, edi
00490f64  sub        eax, esi
00490f66  add        eax, dword ptr [ebp - 0x14]
00490f69  mov        byte ptr [eax], bl
00490f6b  inc        eax
00490f6c  neg        ecx
00490f6e  sbb        ecx, ecx
00490f70  not        ecx
00490f72  and        ecx, eax
00490f74  push       ecx
00490f75  push       edi
00490f76  call       dword ptr [0x4981e4]

// block 00490f7c
00490f7c  test       eax, eax
00490f7e  jne        0x490f8f

// block 00490f80
00490f80  or         dword ptr [ebp - 0xc], 0xffffffff
00490f84  call       0x46e96f

// block 00490f89
00490f89  mov        dword ptr [eax], 0x2a

// block 00490f8f
00490f8f  push       edi
00490f90  call       0x46c941

// block 00490f95
00490f95  pop        ecx

// block 00490f96
00490f96  cmp        dword ptr [ebp - 8], ebx
00490f99  je         0x490fa9

// block 00490f9b
00490f9b  push       dword ptr [ebp - 4]
00490f9e  call       0x46c941

// block 00490fa3
00490fa3  mov        eax, dword ptr [ebp + 8]
00490fa6  pop        ecx
00490fa7  mov        dword ptr [eax], ebx

// block 00490fa9
00490fa9  mov        eax, dword ptr [ebp - 0xc]
00490fac  jmp        0x490e19

// block 00490fb1
00490fb1  push       dword ptr [ebp - 4]
00490fb4  call       0x46c941

// block 00490fb9
00490fb9  mov        eax, dword ptr [ebp + 8]
00490fbc  pop        ecx
00490fbd  mov        dword ptr [eax], ebx

// block 00490fbf
00490fbf  xor        eax, eax
00490fc1  jmp        0x490e19
