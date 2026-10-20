
// block 00470172
00470172  mov        edi, edi
00470174  push       ebp
00470175  mov        ebp, esp
00470177  push       esi
00470178  mov        esi, eax
0047017a  jmp        0x47018f

// block 0047017c
0047017c  mov        ecx, dword ptr [ebp + 0x10]
0047017f  mov        al, byte ptr [ebp + 8]
00470182  dec        dword ptr [ebp + 0xc]
00470185  call       0x47013f

// block 0047018a
0047018a  cmp        dword ptr [esi], -1
0047018d  je         0x470195

// block 0047018f
0047018f  cmp        dword ptr [ebp + 0xc], 0
00470193  jg         0x47017c

// block 00470195
00470195  pop        esi
00470196  pop        ebp
00470197  ret        
