
// block 00435600
00435600  mov        ecx, dword ptr [eax + 4]
00435603  xor        edx, edx
00435605  cmp        ecx, dword ptr [eax]
00435607  setne      dl
0043560a  mov        eax, edx
0043560c  ret        
