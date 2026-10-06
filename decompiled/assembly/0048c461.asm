
// block 0048c461
0048c461  push       ebp
0048c462  mov        ebp, esp
0048c464  sub        esp, 8
0048c467  mov        dword ptr [ebp - 4], edi
0048c46a  mov        dword ptr [ebp - 8], esi
0048c46d  mov        esi, dword ptr [ebp + 0xc]
0048c470  mov        edi, dword ptr [ebp + 8]
0048c473  mov        ecx, dword ptr [ebp + 0x10]
0048c476  shr        ecx, 7
0048c479  jmp        0x48c481

// block 0048c481
0048c481  movdqa     xmm0, xmmword ptr [esi]
0048c485  movdqa     xmm1, xmmword ptr [esi + 0x10]
0048c48a  movdqa     xmm2, xmmword ptr [esi + 0x20]
0048c48f  movdqa     xmm3, xmmword ptr [esi + 0x30]
0048c494  movdqa     xmmword ptr [edi], xmm0
0048c498  movdqa     xmmword ptr [edi + 0x10], xmm1
0048c49d  movdqa     xmmword ptr [edi + 0x20], xmm2
0048c4a2  movdqa     xmmword ptr [edi + 0x30], xmm3
0048c4a7  movdqa     xmm4, xmmword ptr [esi + 0x40]
0048c4ac  movdqa     xmm5, xmmword ptr [esi + 0x50]
0048c4b1  movdqa     xmm6, xmmword ptr [esi + 0x60]
0048c4b6  movdqa     xmm7, xmmword ptr [esi + 0x70]
0048c4bb  movdqa     xmmword ptr [edi + 0x40], xmm4
0048c4c0  movdqa     xmmword ptr [edi + 0x50], xmm5
0048c4c5  movdqa     xmmword ptr [edi + 0x60], xmm6
0048c4ca  movdqa     xmmword ptr [edi + 0x70], xmm7
0048c4cf  lea        esi, [esi + 0x80]
0048c4d5  lea        edi, [edi + 0x80]
0048c4db  dec        ecx
0048c4dc  jne        0x48c481

// block 0048c4de
0048c4de  mov        esi, dword ptr [ebp - 8]
0048c4e1  mov        edi, dword ptr [ebp - 4]
0048c4e4  mov        esp, ebp
0048c4e6  pop        ebp
0048c4e7  ret        
