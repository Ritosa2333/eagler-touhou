
// block 00465620
00465620  mov        eax, dword ptr [esi]
00465622  test       eax, eax
00465624  je         0x465634

// block 00465626
00465626  mov        ecx, dword ptr [eax]
00465628  mov        edx, dword ptr [ecx + 8]
0046562b  push       eax
0046562c  call       edx

// block 0046562e
0046562e  mov        dword ptr [esi], 0

// block 00465634
00465634  ret        
