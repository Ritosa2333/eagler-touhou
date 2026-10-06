
// block 0046d5b7
0046d5b7  mov        edi, edi
0046d5b9  push       ebp
0046d5ba  mov        ebp, esp
0046d5bc  push       ecx
0046d5bd  push       ecx
0046d5be  lea        eax, [ebp - 8]
0046d5c1  push       eax
0046d5c2  call       dword ptr [0x4981dc]

// block 0046d5c8
0046d5c8  mov        eax, dword ptr [ebp - 8]
0046d5cb  mov        ecx, dword ptr [ebp - 4]
0046d5ce  push       0
0046d5d0  add        eax, 0x2ac18000
0046d5d5  push       0x989680
0046d5da  adc        ecx, 0xfe624e21
0046d5e0  push       ecx
0046d5e1  push       eax
0046d5e2  call       0x4767d0

// block 0046d5e7
0046d5e7  cmp        edx, 7
0046d5ea  jl         0x46d5fa

// block 0046d5ec
0046d5ec  jg         0x46d5f5

// block 0046d5ee
0046d5ee  cmp        eax, 0x93406fff
0046d5f3  jbe        0x46d5fa

// block 0046d5f5
0046d5f5  or         eax, 0xffffffff
0046d5f8  mov        edx, eax

// block 0046d5fa
0046d5fa  mov        ecx, dword ptr [ebp + 8]
0046d5fd  test       ecx, ecx
0046d5ff  je         0x46d606

// block 0046d601
0046d601  mov        dword ptr [ecx], eax
0046d603  mov        dword ptr [ecx + 4], edx

// block 0046d606
0046d606  leave      
0046d607  ret        
