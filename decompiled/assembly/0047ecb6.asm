
// block 0047ecb6
0047ecb6  mov        edi, edi
0047ecb8  push       ebp
0047ecb9  mov        ebp, esp
0047ecbb  sub        esp, 0x1c
0047ecbe  push       esi
0047ecbf  mov        esi, dword ptr [0x4b4318]
0047ecc5  xor        ecx, ecx
0047ecc7  cmp        byte ptr [esi], 0x51
0047ecca  mov        dword ptr [ebp - 4], ecx
0047eccd  jne        0x47ecdd

// block 0047eccf
0047eccf  inc        esi
0047ecd0  mov        dword ptr [ebp - 4], 0x49de10
0047ecd7  mov        dword ptr [0x4b4318], esi

// block 0047ecdd
0047ecdd  mov        al, byte ptr [esi]
0047ecdf  cmp        al, cl
0047ece1  jne        0x47ecf5

// block 0047ece3
0047ece3  mov        ecx, dword ptr [ebp + 8]
0047ece6  push       1
0047ece8  call       0x47e1ce

// block 0047eced
0047eced  mov        eax, dword ptr [ebp + 8]
0047ecf0  jmp        0x47ee05

// block 0047ecf5
0047ecf5  cmp        al, 0x30
0047ecf7  jl         0x47ed48

// block 0047ecf9
0047ecf9  cmp        al, 0x39
0047ecfb  jg         0x47ed48

// block 0047ecfd
0047ecfd  movsx      eax, byte ptr [esi]
0047ed00  sub        eax, 0x2f
0047ed03  cdq        
0047ed04  inc        esi
0047ed05  push       edx
0047ed06  mov        dword ptr [0x4b4318], esi
0047ed0c  push       eax
0047ed0d  cmp        dword ptr [ebp - 4], ecx
0047ed10  je         0x47ed2c

// block 0047ed12
0047ed12  lea        ecx, [ebp - 0xc]
0047ed15  call       0x47e692

// block 0047ed1a
0047ed1a  push       eax
0047ed1b  push       dword ptr [ebp - 4]
0047ed1e  lea        eax, [ebp - 0x14]
0047ed21  push       eax
0047ed22  call       0x47ec4a

// block 0047ed27
0047ed27  add        esp, 0xc
0047ed2a  jmp        0x47ed34

// block 0047ed2c
0047ed2c  lea        ecx, [ebp - 0x1c]
0047ed2f  call       0x47e692

// block 0047ed34
0047ed34  mov        edx, dword ptr [eax]
0047ed36  mov        ecx, dword ptr [ebp + 8]
0047ed39  mov        dword ptr [ecx], edx
0047ed3b  mov        eax, dword ptr [eax + 4]
0047ed3e  mov        dword ptr [ecx + 4], eax
0047ed41  mov        eax, ecx
0047ed43  jmp        0x47ee05

// block 0047ed48
0047ed48  push       ebx
0047ed49  push       edi
0047ed4a  xor        edi, edi
0047ed4c  jmp        0x47ed87

// block 0047ed4e
0047ed4e  test       al, al
0047ed50  je         0x47eda7

// block 0047ed52
0047ed52  cmp        al, 0x41
0047ed54  jl         0x47ed98

// block 0047ed56
0047ed56  cmp        al, 0x50
0047ed58  jg         0x47ed98

// block 0047ed5a
0047ed5a  movsx      eax, al
0047ed5d  sub        eax, 0x41
0047ed60  push       0
0047ed62  cdq        
0047ed63  push       0x10
0047ed65  mov        ebx, eax
0047ed67  push       edi
0047ed68  mov        eax, edx
0047ed6a  push       ecx
0047ed6b  mov        dword ptr [ebp - 8], eax
0047ed6e  call       0x483220

// block 0047ed73
0047ed73  add        ebx, eax
0047ed75  mov        eax, dword ptr [ebp - 8]
0047ed78  adc        eax, edx
0047ed7a  inc        esi
0047ed7b  mov        edi, eax
0047ed7d  mov        dword ptr [0x4b4318], esi
0047ed83  mov        al, byte ptr [esi]
0047ed85  mov        ecx, ebx

// block 0047ed87
0047ed87  cmp        al, 0x40
0047ed89  jne        0x47ed4e

// block 0047ed8b
0047ed8b  mov        al, byte ptr [esi]
0047ed8d  inc        esi
0047ed8e  mov        dword ptr [0x4b4318], esi
0047ed94  cmp        al, 0x40
0047ed96  je         0x47edab

// block 0047ed98
0047ed98  push       2

// block 0047ed9a
0047ed9a  mov        ecx, dword ptr [ebp + 8]
0047ed9d  call       0x47e1ce

// block 0047eda2
0047eda2  mov        eax, dword ptr [ebp + 8]
0047eda5  jmp        0x47ee03

// block 0047eda7
0047eda7  push       1
0047eda9  jmp        0x47ed9a

// block 0047edab
0047edab  xor        eax, eax
0047edad  push       edi
0047edae  push       ecx
0047edaf  cmp        byte ptr [ebp + 0xc], al
0047edb2  je         0x47edcd

// block 0047edb4
0047edb4  cmp        dword ptr [ebp - 4], eax
0047edb7  je         0x47edc3

// block 0047edb9
0047edb9  lea        ecx, [ebp - 0x1c]
0047edbc  call       0x47e700

// block 0047edc1
0047edc1  jmp        0x47edda

// block 0047edc3
0047edc3  lea        ecx, [ebp - 0xc]
0047edc6  call       0x47e700

// block 0047edcb
0047edcb  jmp        0x47edf4

// block 0047edcd
0047edcd  cmp        dword ptr [ebp - 4], eax
0047edd0  je         0x47edec

// block 0047edd2
0047edd2  lea        ecx, [ebp - 0x1c]
0047edd5  call       0x47e692

// block 0047edda
0047edda  push       eax
0047eddb  push       dword ptr [ebp - 4]
0047edde  lea        eax, [ebp - 0x14]
0047ede1  push       eax
0047ede2  call       0x47ec4a

// block 0047ede7
0047ede7  add        esp, 0xc
0047edea  jmp        0x47edf4

// block 0047edec
0047edec  lea        ecx, [ebp - 0xc]
0047edef  call       0x47e692

// block 0047edf4
0047edf4  mov        edx, dword ptr [eax]
0047edf6  mov        ecx, dword ptr [ebp + 8]
0047edf9  mov        dword ptr [ecx], edx
0047edfb  mov        eax, dword ptr [eax + 4]
0047edfe  mov        dword ptr [ecx + 4], eax
0047ee01  mov        eax, ecx

// block 0047ee03
0047ee03  pop        edi
0047ee04  pop        ebx

// block 0047ee05
0047ee05  pop        esi
0047ee06  leave      
0047ee07  ret        
