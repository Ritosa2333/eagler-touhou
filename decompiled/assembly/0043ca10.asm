
// block 0043ca10
0043ca10  mov        ecx, dword ptr [eax + 0x18a0]
0043ca16  mov        byte ptr [ecx], dl
0043ca18  inc        dword ptr [eax + 0x18a0]
0043ca1e  mov        ecx, dword ptr [eax + 0x18a0]
0043ca24  sub        ecx, eax
0043ca26  sub        ecx, 0x151c
0043ca2c  xor        eax, eax
0043ca2e  cmp        ecx, 0x1e
0043ca31  setge      al
0043ca34  ret        
