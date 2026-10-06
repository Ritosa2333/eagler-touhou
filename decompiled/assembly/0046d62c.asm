
// block 0046d62c
0046d62c  mov        edi, edi
0046d62e  push       ebp
0046d62f  mov        ebp, esp
0046d631  sub        esp, 0x14
0046d634  push       ebx
0046d635  xor        ebx, ebx
0046d637  push       esi
0046d638  mov        esi, dword ptr [ebp + 8]
0046d63b  mov        dword ptr [ebp - 8], ebx
0046d63e  mov        dword ptr [ebp - 0xc], ebx
0046d641  mov        dword ptr [ebp - 4], ebx
0046d644  cmp        esi, ebx
0046d646  jne        0x46d666

// block 0046d648
0046d648  call       0x46e96f

// block 0046d64d
0046d64d  push       0x16
0046d64f  pop        esi
0046d650  push       ebx
0046d651  push       ebx
0046d652  push       ebx
0046d653  push       ebx
0046d654  push       ebx
0046d655  mov        dword ptr [eax], esi
0046d657  call       0x470f2d

// block 0046d65c
0046d65c  add        esp, 0x14
0046d65f  mov        eax, esi
0046d661  jmp        0x46d8a1

// block 0046d666
0046d666  push       edi
0046d667  push       0x24
0046d669  push       0xff
0046d66e  push       esi
0046d66f  call       0x477420

// block 0046d674
0046d674  mov        edi, dword ptr [ebp + 0xc]
0046d677  add        esp, 0xc
0046d67a  cmp        edi, ebx
0046d67c  jne        0x46d697

// block 0046d67e
0046d67e  call       0x46e96f

// block 0046d683
0046d683  push       0x16
0046d685  pop        esi
0046d686  push       ebx
0046d687  push       ebx
0046d688  push       ebx
0046d689  push       ebx
0046d68a  push       ebx
0046d68b  mov        dword ptr [eax], esi
0046d68d  call       0x470f2d

// block 0046d692
0046d692  add        esp, 0x14
0046d695  jmp        0x46d6bf

// block 0046d697
0046d697  mov        eax, dword ptr [edi + 4]
0046d69a  cmp        eax, ebx
0046d69c  mov        ecx, dword ptr [edi]
0046d69e  jg         0x46d6a6

// block 0046d6a0
0046d6a0  jl         0x46d6b5

// block 0046d6a2
0046d6a2  cmp        ecx, ebx
0046d6a4  jb         0x46d6b5

// block 0046d6a6
0046d6a6  cmp        eax, 7
0046d6a9  jl         0x46d6c6

// block 0046d6ab
0046d6ab  jg         0x46d6b5

// block 0046d6ad
0046d6ad  cmp        ecx, 0x93406fff
0046d6b3  jbe        0x46d6c6

// block 0046d6b5
0046d6b5  call       0x46e96f

// block 0046d6ba
0046d6ba  push       0x16
0046d6bc  pop        esi
0046d6bd  mov        dword ptr [eax], esi

// block 0046d6bf
0046d6bf  mov        eax, esi
0046d6c1  jmp        0x46d8a0

// block 0046d6c6
0046d6c6  call       0x476f80

// block 0046d6cb
0046d6cb  lea        eax, [ebp - 8]
0046d6ce  push       eax
0046d6cf  call       0x4772b2

// block 0046d6d4
0046d6d4  pop        ecx
0046d6d5  test       eax, eax
0046d6d7  je         0x46d6e6

// block 0046d6d9
0046d6d9  push       ebx
0046d6da  push       ebx
0046d6db  push       ebx
0046d6dc  push       ebx
0046d6dd  push       ebx
0046d6de  call       0x470dc6

// block 0046d6e3
0046d6e3  add        esp, 0x14

// block 0046d6e6
0046d6e6  lea        eax, [ebp - 0xc]
0046d6e9  push       eax
0046d6ea  call       0x4772eb

// block 0046d6ef
0046d6ef  pop        ecx
0046d6f0  test       eax, eax
0046d6f2  je         0x46d701

// block 0046d6f4
0046d6f4  push       ebx
0046d6f5  push       ebx
0046d6f6  push       ebx
0046d6f7  push       ebx
0046d6f8  push       ebx
0046d6f9  call       0x470dc6

// block 0046d6fe
0046d6fe  add        esp, 0x14

// block 0046d701
0046d701  lea        eax, [ebp - 4]
0046d704  push       eax
0046d705  call       0x477324

// block 0046d70a
0046d70a  pop        ecx
0046d70b  test       eax, eax
0046d70d  je         0x46d71c

// block 0046d70f
0046d70f  push       ebx
0046d710  push       ebx
0046d711  push       ebx
0046d712  push       ebx
0046d713  push       ebx
0046d714  call       0x470dc6

// block 0046d719
0046d719  add        esp, 0x14

// block 0046d71c
0046d71c  mov        eax, dword ptr [ebp + 0xc]
0046d71f  mov        ecx, dword ptr [eax + 4]
0046d722  cmp        ecx, ebx
0046d724  mov        edi, dword ptr [edi]
0046d726  jl         0x46d796

// block 0046d728
0046d728  jg         0x46d732

// block 0046d72a
0046d72a  cmp        edi, 0x3f480
0046d730  jbe        0x46d796

// block 0046d732
0046d732  mov        eax, dword ptr [ebp - 4]
0046d735  cdq        
0046d736  sub        edi, eax
0046d738  lea        eax, [ebp - 0x14]
0046d73b  push       eax
0046d73c  sbb        ecx, edx
0046d73e  push       esi
0046d73f  mov        dword ptr [ebp - 0x14], edi
0046d742  mov        dword ptr [ebp - 0x10], ecx
0046d745  call       0x477048

// block 0046d74a
0046d74a  pop        ecx
0046d74b  pop        ecx
0046d74c  cmp        eax, ebx
0046d74e  jne        0x46d8a0

// block 0046d754
0046d754  cmp        dword ptr [ebp - 8], ebx
0046d757  je         0x46d89e

// block 0046d75d
0046d75d  push       esi
0046d75e  call       0x477007

// block 0046d763
0046d763  pop        ecx
0046d764  test       eax, eax
0046d766  je         0x46d89e

// block 0046d76c
0046d76c  mov        eax, dword ptr [ebp - 0xc]
0046d76f  cdq        
0046d770  sub        dword ptr [ebp - 0x14], eax
0046d773  lea        eax, [ebp - 0x14]
0046d776  push       eax
0046d777  sbb        dword ptr [ebp - 0x10], edx
0046d77a  push       esi
0046d77b  call       0x477048

// block 0046d780
0046d780  pop        ecx
0046d781  pop        ecx
0046d782  cmp        eax, ebx
0046d784  jne        0x46d8a0

// block 0046d78a
0046d78a  mov        dword ptr [esi + 0x20], 1
0046d791  jmp        0x46d89e

// block 0046d796
0046d796  push       eax
0046d797  push       esi
0046d798  call       0x477048

// block 0046d79d
0046d79d  pop        ecx
0046d79e  pop        ecx
0046d79f  cmp        eax, ebx
0046d7a1  jne        0x46d8a0

// block 0046d7a7
0046d7a7  cmp        dword ptr [ebp - 8], ebx
0046d7aa  je         0x46d7de

// block 0046d7ac
0046d7ac  push       esi
0046d7ad  call       0x477007

// block 0046d7b2
0046d7b2  pop        ecx
0046d7b3  test       eax, eax
0046d7b5  je         0x46d7de

// block 0046d7b7
0046d7b7  mov        ecx, dword ptr [ebp - 0xc]
0046d7ba  mov        eax, dword ptr [ebp - 4]
0046d7bd  add        eax, ecx
0046d7bf  cdq        
0046d7c0  mov        ecx, eax
0046d7c2  mov        eax, edx
0046d7c4  mov        dword ptr [ebp + 8], eax
0046d7c7  mov        eax, dword ptr [esi]
0046d7c9  cdq        
0046d7ca  mov        edi, eax
0046d7cc  sub        edi, ecx
0046d7ce  mov        ecx, dword ptr [ebp + 8]
0046d7d1  mov        ebx, edx
0046d7d3  sbb        ebx, ecx
0046d7d5  mov        dword ptr [esi + 0x20], 1
0046d7dc  jmp        0x46d7ed

// block 0046d7de
0046d7de  mov        eax, dword ptr [esi]
0046d7e0  cdq        
0046d7e1  mov        edi, eax
0046d7e3  mov        eax, dword ptr [ebp - 4]
0046d7e6  mov        ebx, edx
0046d7e8  cdq        
0046d7e9  sub        edi, eax
0046d7eb  sbb        ebx, edx

// block 0046d7ed
0046d7ed  push       0
0046d7ef  push       0x3c
0046d7f1  push       ebx
0046d7f2  push       edi
0046d7f3  call       0x477550

// block 0046d7f8
0046d7f8  mov        dword ptr [esi], eax
0046d7fa  test       eax, eax
0046d7fc  jge        0x46d809

// block 0046d7fe
0046d7fe  add        eax, 0x3c
0046d801  add        edi, -0x3c
0046d804  mov        dword ptr [esi], eax
0046d806  adc        ebx, -1

// block 0046d809
0046d809  push       0
0046d80b  push       0x3c
0046d80d  push       ebx
0046d80e  push       edi
0046d80f  call       0x4774a0

// block 0046d814
0046d814  mov        edi, eax
0046d816  mov        eax, dword ptr [esi + 4]
0046d819  mov        ebx, edx
0046d81b  cdq        
0046d81c  push       0
0046d81e  add        edi, eax
0046d820  push       0x3c
0046d822  adc        ebx, edx
0046d824  push       ebx
0046d825  push       edi
0046d826  call       0x477550

// block 0046d82b
0046d82b  mov        dword ptr [esi + 4], eax
0046d82e  test       eax, eax
0046d830  jge        0x46d83e

// block 0046d832
0046d832  add        eax, 0x3c
0046d835  add        edi, -0x3c
0046d838  mov        dword ptr [esi + 4], eax
0046d83b  adc        ebx, -1

// block 0046d83e
0046d83e  push       0
0046d840  push       0x3c
0046d842  push       ebx
0046d843  push       edi
0046d844  call       0x4774a0

// block 0046d849
0046d849  mov        edi, eax
0046d84b  mov        eax, dword ptr [esi + 8]
0046d84e  mov        ebx, edx
0046d850  cdq        
0046d851  push       0
0046d853  add        edi, eax
0046d855  push       0x18
0046d857  adc        ebx, edx
0046d859  push       ebx
0046d85a  push       edi
0046d85b  call       0x477550

// block 0046d860
0046d860  mov        dword ptr [esi + 8], eax
0046d863  test       eax, eax
0046d865  jge        0x46d873

// block 0046d867
0046d867  add        eax, 0x18
0046d86a  add        edi, -0x18
0046d86d  mov        dword ptr [esi + 8], eax
0046d870  adc        ebx, -1

// block 0046d873
0046d873  push       0
0046d875  push       0x18
0046d877  push       ebx
0046d878  push       edi
0046d879  call       0x4774a0

// block 0046d87e
0046d87e  mov        ecx, eax
0046d880  test       edx, edx
0046d882  jl         0x46d8af

// block 0046d884
0046d884  jg         0x46d88a

// block 0046d886
0046d886  test       ecx, ecx
0046d888  jbe        0x46d8a5

// block 0046d88a
0046d88a  mov        eax, dword ptr [esi + 0x18]
0046d88d  add        eax, ecx
0046d88f  push       7
0046d891  cdq        
0046d892  pop        edi
0046d893  idiv       edi
0046d895  add        dword ptr [esi + 0xc], ecx
0046d898  mov        dword ptr [esi + 0x18], edx

// block 0046d89b
0046d89b  add        dword ptr [esi + 0x1c], ecx

// block 0046d89e
0046d89e  xor        eax, eax

// block 0046d8a0
0046d8a0  pop        edi

// block 0046d8a1
0046d8a1  pop        esi
0046d8a2  pop        ebx
0046d8a3  leave      
0046d8a4  ret        

// block 0046d8a5
0046d8a5  test       edx, edx
0046d8a7  jg         0x46d89e

// block 0046d8a9
0046d8a9  jl         0x46d8af

// block 0046d8ab
0046d8ab  test       ecx, ecx
0046d8ad  jae        0x46d89e

// block 0046d8af
0046d8af  mov        eax, dword ptr [esi + 0x18]
0046d8b2  lea        eax, [ecx + eax + 7]
0046d8b6  cdq        
0046d8b7  push       7
0046d8b9  pop        edi
0046d8ba  idiv       edi
0046d8bc  add        dword ptr [esi + 0xc], ecx
0046d8bf  mov        eax, dword ptr [esi + 0xc]
0046d8c2  mov        dword ptr [esi + 0x18], edx
0046d8c5  test       eax, eax
0046d8c7  jg         0x46d89b

// block 0046d8c9
0046d8c9  add        ecx, 0x16d
0046d8cf  add        dword ptr [esi + 0x1c], ecx
0046d8d2  add        eax, 0x1f
0046d8d5  dec        dword ptr [esi + 0x14]
0046d8d8  mov        dword ptr [esi + 0xc], eax
0046d8db  mov        dword ptr [esi + 0x10], 0xb
0046d8e2  jmp        0x46d89e
