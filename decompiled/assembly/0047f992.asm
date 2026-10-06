
// block 0047f992
0047f992  mov        edi, edi
0047f994  push       ebp
0047f995  mov        ebp, esp
0047f997  sub        esp, 0x6c
0047f99a  mov        eax, dword ptr [0x4ad138]
0047f99f  xor        eax, ebp
0047f9a1  mov        dword ptr [ebp - 4], eax
0047f9a4  push       ebx
0047f9a5  xor        ebx, ebx
0047f9a7  push       esi
0047f9a8  mov        esi, dword ptr [ebp + 8]
0047f9ab  mov        byte ptr [esi + 4], bl
0047f9ae  and        dword ptr [esi + 4], 0xffff00ff
0047f9b5  mov        dword ptr [esi], ebx
0047f9b7  mov        byte ptr [0x4b4331], 1
0047f9be  mov        dword ptr [ebp - 0x20], 1
0047f9c5  cmp        byte ptr [esi + 4], bl
0047f9c8  jne        0x47fb40

// block 0047f9ce
0047f9ce  push       edi
0047f9cf  mov        edi, 0xffff0000

// block 0047f9d4
0047f9d4  mov        eax, dword ptr [0x4b4318]
0047f9d9  mov        cl, byte ptr [eax]
0047f9db  cmp        cl, bl
0047f9dd  je         0x47fb3f

// block 0047f9e3
0047f9e3  cmp        cl, 0x40
0047f9e6  je         0x47fb3f

// block 0047f9ec
0047f9ec  cmp        dword ptr [ebp - 0x20], ebx
0047f9ef  je         0x47f9f6

// block 0047f9f1
0047f9f1  mov        dword ptr [ebp - 0x20], ebx
0047f9f4  jmp        0x47fa04

// block 0047f9f6
0047f9f6  push       0x2c
0047f9f8  mov        ecx, esi
0047f9fa  call       0x47e9f6

// block 0047f9ff
0047f9ff  mov        eax, dword ptr [0x4b4318]

// block 0047fa04
0047fa04  mov        dl, byte ptr [eax]
0047fa06  movsx      ecx, dl
0047fa09  sub        ecx, 0x30
0047fa0c  cmp        ecx, 9
0047fa0f  ja         0x47fa2c

// block 0047fa11
0047fa11  inc        eax
0047fa12  mov        dword ptr [0x4b4318], eax
0047fa17  push       ecx
0047fa18  mov        ecx, dword ptr [0x4b4314]
0047fa1e  lea        eax, [ebp - 0x6c]
0047fa21  push       eax
0047fa22  call       0x47e378

// block 0047fa27
0047fa27  jmp        0x47fb2e

// block 0047fa2c
0047fa2c  and        dword ptr [ebp - 0x18], edi
0047fa2f  mov        dword ptr [ebp - 0x34], eax
0047fa32  mov        dword ptr [ebp - 0x1c], ebx
0047fa35  cmp        dl, 0x58
0047fa38  jne        0x47fa52

// block 0047fa3a
0047fa3a  inc        eax
0047fa3b  mov        dword ptr [0x4b4318], eax
0047fa40  push       0x49de5c

// block 0047fa45
0047fa45  lea        ecx, [ebp - 0x1c]
0047fa48  call       0x47e896

// block 0047fa4d
0047fa4d  jmp        0x47fb0a

// block 0047fa52
0047fa52  cmp        dl, 0x24
0047fa55  jne        0x47fa6f

// block 0047fa57
0047fa57  inc        eax
0047fa58  cmp        byte ptr [eax], dl
0047fa5a  je         0x47fa6f

// block 0047fa5c
0047fa5c  mov        dword ptr [0x4b4318], eax
0047fa61  lea        eax, [ebp - 0x3c]
0047fa64  push       eax
0047fa65  call       0x47f5d0

// block 0047fa6a
0047fa6a  jmp        0x47fafe

// block 0047fa6f
0047fa6f  cmp        dl, 0x3f
0047fa72  jne        0x47faea

// block 0047fa74
0047fa74  lea        eax, [ebp - 0x28]
0047fa77  push       eax
0047fa78  call       0x47f57e

// block 0047fa7d
0047fa7d  test       dword ptr [0x4b4328], 0x4000
0047fa87  pop        ecx
0047fa88  je         0x47fad8

// block 0047fa8a
0047fa8a  push       0x10
0047fa8c  lea        eax, [ebp - 0x14]
0047fa8f  push       eax
0047fa90  lea        ecx, [ebp - 0x28]
0047fa93  call       0x47e213

// block 0047fa98
0047fa98  lea        eax, [ebp - 0x14]
0047fa9b  push       eax
0047fa9c  call       0x46d114

// block 0047faa1
0047faa1  push       eax
0047faa2  call       dword ptr [0x4b432c]

// block 0047faa8
0047faa8  pop        ecx
0047faa9  pop        ecx
0047faaa  cmp        eax, ebx
0047faac  je         0x47fab1

// block 0047faae
0047faae  push       eax
0047faaf  jmp        0x47fa45

// block 0047fab1
0047fab1  push       0x49dea8
0047fab6  lea        eax, [ebp - 0x64]
0047fab9  push       eax
0047faba  lea        eax, [ebp - 0x28]
0047fabd  push       eax
0047fabe  lea        eax, [ebp - 0x54]

// block 0047fac1
0047fac1  push       0x49de94
0047fac6  push       eax
0047fac7  call       0x47ec4a

// block 0047facc
0047facc  add        esp, 0xc
0047facf  mov        ecx, eax
0047fad1  call       0x47ec92

// block 0047fad6
0047fad6  jmp        0x47faff

// block 0047fad8
0047fad8  push       0x49dea8
0047fadd  lea        eax, [ebp - 0x44]
0047fae0  push       eax
0047fae1  lea        eax, [ebp - 0x28]
0047fae4  push       eax
0047fae5  lea        eax, [ebp - 0x4c]
0047fae8  jmp        0x47fac1

// block 0047faea
0047faea  and        dword ptr [ebp - 0x2c], edi
0047faed  lea        eax, [ebp - 0x30]
0047faf0  push       eax
0047faf1  lea        eax, [ebp - 0x5c]
0047faf4  push       eax
0047faf5  mov        dword ptr [ebp - 0x30], ebx
0047faf8  call       0x4826a7

// block 0047fafd
0047fafd  pop        ecx

// block 0047fafe
0047fafe  pop        ecx

// block 0047faff
0047faff  mov        ecx, dword ptr [eax]
0047fb01  mov        eax, dword ptr [eax + 4]
0047fb04  mov        dword ptr [ebp - 0x18], eax
0047fb07  mov        dword ptr [ebp - 0x1c], ecx

// block 0047fb0a
0047fb0a  mov        eax, dword ptr [0x4b4318]
0047fb0f  sub        eax, dword ptr [ebp - 0x34]
0047fb12  cmp        eax, 1
0047fb15  jle        0x47fb2b

// block 0047fb17
0047fb17  mov        ecx, dword ptr [0x4b4314]
0047fb1d  cmp        dword ptr [ecx], 9
0047fb20  je         0x47fb2b

// block 0047fb22
0047fb22  lea        eax, [ebp - 0x1c]
0047fb25  push       eax
0047fb26  call       0x47e32e

// block 0047fb2b
0047fb2b  lea        eax, [ebp - 0x1c]

// block 0047fb2e
0047fb2e  push       eax
0047fb2f  mov        ecx, esi
0047fb31  call       0x47e7bf

// block 0047fb36
0047fb36  cmp        byte ptr [esi + 4], bl
0047fb39  je         0x47f9d4

// block 0047fb3f
0047fb3f  pop        edi

// block 0047fb40
0047fb40  mov        ecx, dword ptr [ebp - 4]
0047fb43  mov        eax, esi
0047fb45  pop        esi
0047fb46  mov        byte ptr [0x4b4331], bl
0047fb4c  xor        ecx, ebp
0047fb4e  pop        ebx
0047fb4f  call       0x46c932

// block 0047fb54
0047fb54  leave      
0047fb55  ret        
