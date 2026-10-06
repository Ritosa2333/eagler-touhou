
// block 004921ff
004921ff  call       0x475467

// block 00492204
00492204  xor        ecx, ecx
00492206  cmp        dword ptr [eax + 0x90], ecx
0049220c  setne      cl
0049220f  mov        al, cl
00492211  ret        
