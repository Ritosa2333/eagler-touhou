
// block 00482f20
00482f20  mov        edi, edi
00482f22  push       ebp
00482f23  mov        ebp, esp
00482f25  sub        esp, 0x14
00482f28  push       ebx
00482f29  push       esi
00482f2a  push       edi
00482f2b  call       0x4751d5

// block 00482f30
00482f30  and        dword ptr [ebp - 4], 0
00482f34  cmp        dword ptr [0x4b4380], 0
00482f3b  mov        ebx, eax
00482f3d  jne        0x482fd1

// block 00482f43
00482f43  push       0x49e1bc
00482f48  call       dword ptr [0x498058]

// block 00482f4e
00482f4e  mov        edi, eax
00482f50  test       edi, edi
00482f52  je         0x483082

// block 00482f58
00482f58  mov        esi, dword ptr [0x498170]
00482f5e  push       0x49e1b0
00482f63  push       edi
00482f64  call       esi

// block 00482f66
00482f66  test       eax, eax
00482f68  je         0x483082

// block 00482f6e
00482f6e  push       eax
00482f6f  call       0x475163

// block 00482f74
00482f74  mov        dword ptr [esp], 0x49e1a0
00482f7b  push       edi
00482f7c  mov        dword ptr [0x4b4380], eax
00482f81  call       esi

// block 00482f83
00482f83  push       eax
00482f84  call       0x475163

// block 00482f89
00482f89  mov        dword ptr [esp], 0x49e18c
00482f90  push       edi
00482f91  mov        dword ptr [0x4b4384], eax
00482f96  call       esi

// block 00482f98
00482f98  push       eax
00482f99  call       0x475163

// block 00482f9e
00482f9e  mov        dword ptr [esp], 0x49e170
00482fa5  push       edi
00482fa6  mov        dword ptr [0x4b4388], eax
00482fab  call       esi

// block 00482fad
00482fad  push       eax
00482fae  call       0x475163

// block 00482fb3
00482fb3  pop        ecx
00482fb4  mov        dword ptr [0x4b4390], eax
00482fb9  test       eax, eax
00482fbb  je         0x482fd1

// block 00482fbd
00482fbd  push       0x49e158
00482fc2  push       edi
00482fc3  call       esi

// block 00482fc5
00482fc5  push       eax
00482fc6  call       0x475163

// block 00482fcb
00482fcb  pop        ecx
00482fcc  mov        dword ptr [0x4b438c], eax

// block 00482fd1
00482fd1  mov        eax, dword ptr [0x4b438c]
00482fd6  cmp        eax, ebx
00482fd8  je         0x483029

// block 00482fda
00482fda  cmp        dword ptr [0x4b4390], ebx
00482fe0  je         0x483029

// block 00482fe2
00482fe2  push       eax
00482fe3  call       0x4751de

// block 00482fe8
00482fe8  push       dword ptr [0x4b4390]
00482fee  mov        esi, eax
00482ff0  call       0x4751de

// block 00482ff5
00482ff5  pop        ecx
00482ff6  pop        ecx
00482ff7  mov        edi, eax
00482ff9  test       esi, esi
00482ffb  je         0x483029

// block 00482ffd
00482ffd  test       edi, edi
00482fff  je         0x483029

// block 00483001
00483001  call       esi

// block 00483003
00483003  test       eax, eax
00483005  je         0x483020

// block 00483007
00483007  lea        ecx, [ebp - 8]
0048300a  push       ecx
0048300b  push       0xc
0048300d  lea        ecx, [ebp - 0x14]
00483010  push       ecx
00483011  push       1
00483013  push       eax
00483014  call       edi

// block 00483016
00483016  test       eax, eax
00483018  je         0x483020

// block 0048301a
0048301a  test       byte ptr [ebp - 0xc], 1
0048301e  jne        0x483029

// block 00483020
00483020  or         dword ptr [ebp + 0x10], 0x200000
00483027  jmp        0x483062

// block 00483029
00483029  mov        eax, dword ptr [0x4b4384]
0048302e  cmp        eax, ebx
00483030  je         0x483062

// block 00483032
00483032  push       eax
00483033  call       0x4751de

// block 00483038
00483038  pop        ecx
00483039  test       eax, eax
0048303b  je         0x483062

// block 0048303d
0048303d  call       eax

// block 0048303f
0048303f  mov        dword ptr [ebp - 4], eax
00483042  test       eax, eax
00483044  je         0x483062

// block 00483046
00483046  mov        eax, dword ptr [0x4b4388]
0048304b  cmp        eax, ebx
0048304d  je         0x483062

// block 0048304f
0048304f  push       eax
00483050  call       0x4751de

// block 00483055
00483055  pop        ecx
00483056  test       eax, eax
00483058  je         0x483062

// block 0048305a
0048305a  push       dword ptr [ebp - 4]
0048305d  call       eax

// block 0048305f
0048305f  mov        dword ptr [ebp - 4], eax

// block 00483062
00483062  push       dword ptr [0x4b4380]
00483068  call       0x4751de

// block 0048306d
0048306d  pop        ecx
0048306e  test       eax, eax
00483070  je         0x483082

// block 00483072
00483072  push       dword ptr [ebp + 0x10]
00483075  push       dword ptr [ebp + 0xc]
00483078  push       dword ptr [ebp + 8]
0048307b  push       dword ptr [ebp - 4]
0048307e  call       eax

// block 00483080
00483080  jmp        0x483084

// block 00483082
00483082  xor        eax, eax

// block 00483084
00483084  pop        edi
00483085  pop        esi
00483086  pop        ebx
00483087  leave      
00483088  ret        
