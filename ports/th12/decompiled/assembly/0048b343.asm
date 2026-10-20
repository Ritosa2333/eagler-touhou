
// block 0048b343
0048b343  push       ebp
0048b344  mov        ebp, esp
0048b346  sub        esp, 4
0048b349  mov        dword ptr [ebp - 4], edi
0048b34c  mov        edi, dword ptr [ebp + 8]
0048b34f  mov        ecx, dword ptr [ebp + 0xc]
0048b352  shr        ecx, 7
0048b355  pxor       xmm0, xmm0
0048b359  jmp        0x48b363

// block 0048b363
0048b363  movdqa     xmmword ptr [edi], xmm0
0048b367  movdqa     xmmword ptr [edi + 0x10], xmm0
0048b36c  movdqa     xmmword ptr [edi + 0x20], xmm0
0048b371  movdqa     xmmword ptr [edi + 0x30], xmm0
0048b376  movdqa     xmmword ptr [edi + 0x40], xmm0
0048b37b  movdqa     xmmword ptr [edi + 0x50], xmm0
0048b380  movdqa     xmmword ptr [edi + 0x60], xmm0
0048b385  movdqa     xmmword ptr [edi + 0x70], xmm0
0048b38a  lea        edi, [edi + 0x80]
0048b390  dec        ecx
0048b391  jne        0x48b363

// block 0048b393
0048b393  mov        edi, dword ptr [ebp - 4]
0048b396  mov        esp, ebp
0048b398  pop        ebp
0048b399  ret        
