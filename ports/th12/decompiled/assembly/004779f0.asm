
// block 004779f0
004779f0  mov        edi, edi
004779f2  push       ebp
004779f3  mov        ebp, esp
004779f5  mov        eax, dword ptr [ebp + 8]
004779f8  mov        ecx, dword ptr [eax + 0x3c]
004779fb  add        ecx, eax
004779fd  movzx      eax, word ptr [ecx + 0x14]
00477a01  push       ebx
00477a02  push       esi
00477a03  movzx      esi, word ptr [ecx + 6]
00477a07  xor        edx, edx
00477a09  push       edi
00477a0a  lea        eax, [eax + ecx + 0x18]
00477a0e  test       esi, esi
00477a10  jbe        0x477a2d

// block 00477a12
00477a12  mov        edi, dword ptr [ebp + 0xc]

// block 00477a15
00477a15  mov        ecx, dword ptr [eax + 0xc]
00477a18  cmp        edi, ecx
00477a1a  jb         0x477a25

// block 00477a1c
00477a1c  mov        ebx, dword ptr [eax + 8]
00477a1f  add        ebx, ecx
00477a21  cmp        edi, ebx
00477a23  jb         0x477a2f

// block 00477a25
00477a25  inc        edx
00477a26  add        eax, 0x28
00477a29  cmp        edx, esi
00477a2b  jb         0x477a15

// block 00477a2d
00477a2d  xor        eax, eax

// block 00477a2f
00477a2f  pop        edi
00477a30  pop        esi
00477a31  pop        ebx
00477a32  pop        ebp
00477a33  ret        
