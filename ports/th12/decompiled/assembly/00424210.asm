
// block 00424210
00424210  xor        eax, eax
00424212  test       edx, edx
00424214  jle        0x42422c

// block 00424216
00424216  lea        ecx, [edi + 4]
00424219  lea        esp, [esp]

// block 00424220
00424220  cmp        dword ptr [ecx], esi
00424222  je         0x42422f

// block 00424224
00424224  inc        eax
00424225  add        ecx, 8
00424228  cmp        eax, edx
0042422a  jl         0x424220

// block 0042422c
0042422c  xor        eax, eax
0042422e  ret        

// block 0042422f
0042422f  mov        eax, dword ptr [edi + eax*8]
00424232  ret        
