
// block 00455580
00455580  mov        ecx, dword ptr [eax]
00455582  add        ecx, 0xffffd8f0
00455588  cmp        ecx, 9
0045558b  ja         0x4555bd

// block 0045558d
0045558d  jmp        dword ptr [ecx*4 + 0x4555c0]

// block 00455594
00455594  lea        eax, [edx + 0x3fc]
0045559a  ret        

// block 0045559b
0045559b  lea        eax, [edx + 0x400]
004555a1  ret        

// block 004555a2
004555a2  lea        eax, [edx + 0x404]
004555a8  ret        

// block 004555a9
004555a9  lea        eax, [edx + 0x408]
004555af  ret        

// block 004555b0
004555b0  lea        eax, [edx + 0x41c]
004555b6  ret        

// block 004555b7
004555b7  lea        eax, [edx + 0x420]

// block 004555bd
004555bd  ret        
