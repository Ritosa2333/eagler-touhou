
// block 00431830
00431830  test       dword ptr [edi + 0x590], 0x8000
0043183a  je         0x431854

// block 0043183c
0043183c  lea        eax, [esi + esi*2 + 0x102]
00431843  lea        ecx, [edi + eax*8]
00431846  push       ecx
00431847  call       dword ptr [0x498088]

// block 0043184d
0043184d  inc        byte ptr [esi + edi + 0x930]

// block 00431854
00431854  ret        
