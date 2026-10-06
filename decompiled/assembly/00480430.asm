
// block 00480430
00480430  mov        edi, edi
00480432  push       ebp
00480433  mov        ebp, esp
00480435  sub        esp, 0x18
00480438  push       ebx
00480439  push       esi
0048043a  mov        esi, dword ptr [ebp + 8]
0048043d  push       edi
0048043e  xor        ebx, ebx
00480440  push       ebx
00480441  lea        eax, [ebp - 8]
00480444  push       1
00480446  mov        byte ptr [esi + 4], bl
00480449  and        dword ptr [esi + 4], 0xffff00ff
00480450  push       eax
00480451  mov        dword ptr [esi], ebx
00480453  call       0x48023a

// block 00480458
00480458  add        esp, 0xc
0048045b  push       eax
0048045c  mov        ecx, esi
0048045e  call       0x47dde0

// block 00480463
00480463  mov        edi, 0x49df1c
00480468  cmp        byte ptr [esi + 4], bl
0048046b  jne        0x4804a6

// block 0048046d
0048046d  mov        eax, dword ptr [0x4b4318]
00480472  mov        al, byte ptr [eax]
00480474  cmp        al, bl
00480476  je         0x4804a6

// block 00480478
00480478  cmp        al, 0x40
0048047a  je         0x4804b1

// block 0048047c
0048047c  push       esi
0048047d  lea        eax, [ebp - 8]
00480480  push       eax
00480481  push       edi
00480482  lea        eax, [ebp - 0x10]
00480485  push       eax
00480486  lea        eax, [ebp - 0x18]
00480489  push       eax
0048048a  call       0x4814b0

// block 0048048f
0048048f  pop        ecx
00480490  mov        ecx, eax
00480492  call       0x47ec92

// block 00480497
00480497  mov        ecx, eax
00480499  call       0x47e9ae

// block 0048049e
0048049e  push       eax
0048049f  mov        ecx, esi
004804a1  call       0x47dde0

// block 004804a6
004804a6  mov        eax, dword ptr [0x4b4318]
004804ab  mov        al, byte ptr [eax]
004804ad  cmp        al, 0x40
004804af  jne        0x4804b9

// block 004804b1
004804b1  inc        dword ptr [0x4b4318]
004804b7  jmp        0x4804fa

// block 004804b9
004804b9  cmp        al, bl
004804bb  je         0x4804c1

// block 004804bd
004804bd  push       2
004804bf  jmp        0x4804c7

// block 004804c1
004804c1  cmp        dword ptr [esi], ebx
004804c3  jne        0x4804d0

// block 004804c5
004804c5  push       1

// block 004804c7
004804c7  mov        ecx, esi
004804c9  call       0x47e2f7

// block 004804ce
004804ce  jmp        0x4804fa

// block 004804d0
004804d0  push       esi
004804d1  lea        eax, [ebp - 0x18]
004804d4  push       eax
004804d5  push       edi
004804d6  lea        eax, [ebp - 0x10]
004804d9  push       eax
004804da  push       1
004804dc  lea        ecx, [ebp - 8]
004804df  call       0x47e1ce

// block 004804e4
004804e4  mov        ecx, eax
004804e6  call       0x47ec92

// block 004804eb
004804eb  mov        ecx, eax
004804ed  call       0x47e9ae

// block 004804f2
004804f2  push       eax
004804f3  mov        ecx, esi
004804f5  call       0x47dde0

// block 004804fa
004804fa  pop        edi
004804fb  mov        eax, esi
004804fd  pop        esi
004804fe  pop        ebx
004804ff  leave      
00480500  ret        
