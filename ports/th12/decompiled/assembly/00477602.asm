
// block 00477602
00477602  mov        edi, edi
00477604  push       ebp
00477605  mov        ebp, esp
00477607  push       ecx
00477608  push       ebx
00477609  push       esi
0047760a  mov        esi, dword ptr [ebp + 8]
0047760d  xor        ebx, ebx
0047760f  mov        dword ptr [ebp - 4], ebx
00477612  cmp        esi, ebx
00477614  jne        0x477634

// block 00477616
00477616  call       0x46e96f

// block 0047761b
0047761b  push       0x16
0047761d  pop        esi
0047761e  push       ebx
0047761f  push       ebx
00477620  push       ebx
00477621  push       ebx
00477622  push       ebx
00477623  mov        dword ptr [eax], esi
00477625  call       0x470f2d

// block 0047762a
0047762a  add        esp, 0x14

// block 0047762d
0047762d  mov        eax, esi
0047762f  jmp        0x477731

// block 00477634
00477634  push       0x24
00477636  push       0xff
0047763b  push       esi
0047763c  call       0x477420

// block 00477641
00477641  mov        eax, dword ptr [ebp + 0xc]
00477644  add        esp, 0xc
00477647  cmp        eax, ebx
00477649  je         0x477616

// block 0047764b
0047764b  mov        ecx, dword ptr [eax]
0047764d  cmp        ecx, 0xffff5740
00477653  jge        0x477661

// block 00477655
00477655  call       0x46e96f

// block 0047765a
0047765a  push       0x16
0047765c  pop        esi
0047765d  mov        dword ptr [eax], esi
0047765f  jmp        0x47762d

// block 00477661
00477661  mov        eax, ecx
00477663  cdq        
00477664  push       edi
00477665  mov        edi, 0x7861f80
0047766a  idiv       edi
0047766c  mov        edx, eax
0047766e  imul       edx, edx, 0xf879e080
00477674  add        ecx, edx
00477676  lea        edx, [eax*4 + 0x46]
0047767d  mov        eax, 0x1e13380
00477682  cmp        ecx, eax
00477684  jl         0x4776a5

// block 00477686
00477686  sub        ecx, eax
00477688  inc        edx
00477689  cmp        ecx, eax
0047768b  jl         0x4776a5

// block 0047768d
0047768d  sub        ecx, eax
0047768f  mov        eax, 0x1e28500
00477694  inc        edx
00477695  cmp        ecx, eax
00477697  jl         0x47769e

// block 00477699
00477699  inc        edx
0047769a  sub        ecx, eax
0047769c  jmp        0x4776a5

// block 0047769e
0047769e  mov        dword ptr [ebp - 4], 1

// block 004776a5
004776a5  mov        eax, ecx
004776a7  mov        dword ptr [esi + 0x14], edx
004776aa  cdq        
004776ab  mov        edi, 0x15180
004776b0  idiv       edi
004776b2  mov        edi, 0x4ae29c
004776b7  mov        dword ptr [esi + 0x1c], eax
004776ba  imul       eax, eax, 0xfffeae80
004776c0  add        ecx, eax
004776c2  cmp        dword ptr [ebp - 4], ebx
004776c5  jne        0x4776cc

// block 004776c7
004776c7  mov        edi, 0x4ae2d0

// block 004776cc
004776cc  mov        eax, dword ptr [esi + 0x1c]
004776cf  xor        edx, edx
004776d1  inc        edx
004776d2  cmp        dword ptr [edi + 4], eax
004776d5  jge        0x4776e1

// block 004776d7
004776d7  mov        ebx, eax

// block 004776d9
004776d9  inc        edx
004776da  cmp        dword ptr [edi + edx*4], ebx
004776dd  jl         0x4776d9

// block 004776df
004776df  xor        ebx, ebx

// block 004776e1
004776e1  dec        edx
004776e2  mov        dword ptr [esi + 0x10], edx
004776e5  sub        eax, dword ptr [edi + edx*4]
004776e8  mov        edi, 0x15180
004776ed  mov        dword ptr [esi + 0xc], eax
004776f0  mov        eax, dword ptr [ebp + 0xc]
004776f3  mov        eax, dword ptr [eax]
004776f5  cdq        
004776f6  idiv       edi
004776f8  push       7
004776fa  pop        edi
004776fb  push       0x3c
004776fd  mov        dword ptr [esi + 0x20], ebx
00477700  add        eax, 4
00477703  cdq        
00477704  idiv       edi
00477706  mov        eax, ecx
00477708  mov        edi, 0xe10
0047770d  mov        dword ptr [esi + 0x18], edx
00477710  cdq        
00477711  idiv       edi
00477713  pop        edi
00477714  mov        dword ptr [esi + 8], eax
00477717  imul       eax, eax, 0xfffff1f0
0047771d  add        ecx, eax
0047771f  mov        eax, ecx
00477721  cdq        
00477722  idiv       edi
00477724  pop        edi
00477725  mov        dword ptr [esi + 4], eax
00477728  imul       eax, eax, 0x3c
0047772b  sub        ecx, eax
0047772d  mov        dword ptr [esi], ecx
0047772f  xor        eax, eax

// block 00477731
00477731  pop        esi
00477732  pop        ebx
00477733  leave      
00477734  ret        
