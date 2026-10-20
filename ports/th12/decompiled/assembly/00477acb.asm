
// block 00477acb
00477acb  mov        eax, dword ptr [ebp - 0x14]
00477ace  mov        ecx, dword ptr [eax]
00477ad0  mov        eax, dword ptr [ecx]
00477ad2  xor        edx, edx
00477ad4  cmp        eax, 0xc0000005
00477ad9  sete       dl
00477adc  mov        eax, edx
00477ade  ret        
