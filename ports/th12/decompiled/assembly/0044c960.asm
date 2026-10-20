
// block 0044c960
0044c960  test       edx, edx
0044c962  jne        0x44c969

// block 0044c964
0044c964  xor        eax, eax
0044c966  ret        4

// block 0044c969
0044c969  mov        eax, dword ptr [0x4cc548]
0044c96e  push       ebx
0044c96f  push       ebp
0044c970  push       esi
0044c971  xor        ebx, ebx
0044c973  push       edi

// block 0044c974
0044c974  mov        edi, eax
0044c976  xor        esi, esi
0044c978  sub        edi, edx
0044c97a  lea        ebx, [ebx]

// block 0044c980
0044c980  lea        ecx, [esi + edx]
0044c983  lea        ebp, [edi + ecx]
0044c986  and        ebp, 0x1fff
0044c98c  movzx      ebp, byte ptr [ebp + 0x4cc550]
0044c993  and        ecx, 0x1fff
0044c999  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044c9a0  sub        ecx, ebp
0044c9a2  jne        0x44ca91

// block 0044c9a8
0044c9a8  lea        ecx, [esi + eax + 1]
0044c9ac  and        ecx, 0x1fff
0044c9b2  movzx      ebp, byte ptr [ecx + 0x4cc550]
0044c9b9  lea        ecx, [esi + edx + 1]
0044c9bd  and        ecx, 0x1fff
0044c9c3  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044c9ca  sub        ecx, ebp
0044c9cc  jne        0x44ca7c

// block 0044c9d2
0044c9d2  lea        ecx, [esi + eax + 2]
0044c9d6  and        ecx, 0x1fff
0044c9dc  movzx      ebp, byte ptr [ecx + 0x4cc550]
0044c9e3  lea        ecx, [esi + edx + 2]
0044c9e7  and        ecx, 0x1fff
0044c9ed  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044c9f4  sub        ecx, ebp
0044c9f6  jne        0x44ca7f

// block 0044c9fc
0044c9fc  lea        ecx, [esi + eax + 3]
0044ca00  and        ecx, 0x1fff
0044ca06  movzx      ebp, byte ptr [ecx + 0x4cc550]
0044ca0d  lea        ecx, [esi + edx + 3]
0044ca11  and        ecx, 0x1fff
0044ca17  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044ca1e  sub        ecx, ebp
0044ca20  jne        0x44ca84

// block 0044ca22
0044ca22  lea        ecx, [esi + eax + 4]
0044ca26  and        ecx, 0x1fff
0044ca2c  movzx      ebp, byte ptr [ecx + 0x4cc550]
0044ca33  lea        ecx, [esi + edx + 4]
0044ca37  and        ecx, 0x1fff
0044ca3d  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044ca44  sub        ecx, ebp
0044ca46  jne        0x44ca89

// block 0044ca48
0044ca48  lea        ecx, [esi + eax + 5]
0044ca4c  and        ecx, 0x1fff
0044ca52  movzx      ebp, byte ptr [ecx + 0x4cc550]
0044ca59  lea        ecx, [esi + edx + 5]
0044ca5d  and        ecx, 0x1fff
0044ca63  movzx      ecx, byte ptr [ecx + 0x4cc550]
0044ca6a  sub        ecx, ebp
0044ca6c  jne        0x44ca8e

// block 0044ca6e
0044ca6e  add        esi, 6
0044ca71  cmp        esi, 0x12
0044ca74  jl         0x44c980

// block 0044ca7a
0044ca7a  jmp        0x44ca91

// block 0044ca7c
0044ca7c  inc        esi
0044ca7d  jmp        0x44ca91

// block 0044ca7f
0044ca7f  add        esi, 2
0044ca82  jmp        0x44ca91

// block 0044ca84
0044ca84  add        esi, 3
0044ca87  jmp        0x44ca91

// block 0044ca89
0044ca89  add        esi, 4
0044ca8c  jmp        0x44ca91

// block 0044ca8e
0044ca8e  add        esi, 5

// block 0044ca91
0044ca91  cmp        esi, ebx
0044ca93  jl         0x44caa2

// block 0044ca95
0044ca95  cmp        esi, 0x12
0044ca98  mov        edi, dword ptr [esp + 0x14]
0044ca9c  mov        ebx, esi
0044ca9e  mov        dword ptr [edi], eax
0044caa0  jge        0x44cac6

// block 0044caa2
0044caa2  test       ecx, ecx
0044caa4  lea        ecx, [eax + eax*2]
0044caa7  jl         0x44cab2

// block 0044caa9
0044caa9  lea        ecx, [ecx*4 + 0x4b4548]
0044cab0  jmp        0x44cab9

// block 0044cab2
0044cab2  lea        ecx, [ecx*4 + 0x4b4544]

// block 0044cab9
0044cab9  mov        esi, dword ptr [ecx]
0044cabb  test       esi, esi
0044cabd  je         0x44cad4

// block 0044cabf
0044cabf  mov        eax, esi
0044cac1  jmp        0x44c974

// block 0044cac6
0044cac6  call       0x44cba0

// block 0044cacb
0044cacb  pop        edi
0044cacc  mov        eax, esi
0044cace  pop        esi
0044cacf  pop        ebp
0044cad0  pop        ebx
0044cad1  ret        4

// block 0044cad4
0044cad4  mov        dword ptr [ecx], edx
0044cad6  lea        ecx, [edx + edx*2]
0044cad9  pop        edi
0044cada  add        ecx, ecx
0044cadc  add        ecx, ecx
0044cade  pop        esi
0044cadf  mov        dword ptr [ecx + 0x4b4540], eax
0044cae5  pop        ebp
0044cae6  mov        eax, ebx
0044cae8  mov        dword ptr [ecx + 0x4b4548], 0
0044caf2  mov        dword ptr [ecx + 0x4b4544], 0
0044cafc  pop        ebx
0044cafd  ret        4
