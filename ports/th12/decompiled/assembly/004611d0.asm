
// block 004611d0
004611d0  test       dword ptr [0x4cee78], 0x8000
004611da  push       esi
004611db  mov        esi, eax
004611dd  je         0x4611f0

// block 004611df
004611df  push       0x4cf1d0
004611e4  call       dword ptr [0x498088]

// block 004611ea
004611ea  inc        byte ptr [0x4cf221]

// block 004611f0
004611f0  imul       esi, esi, 0x4b4
004611f6  mov        esi, dword ptr [esi + edi + 0x8856e4]
004611fd  test       esi, esi
004611ff  je         0x46122a

// block 00461201
00461201  test       dword ptr [esi + 0x47c], 0x10000000
0046120b  jne        0x461223

// block 0046120d
0046120d  mov        eax, dword ptr [esi + 0x48c]
00461213  test       eax, eax
00461215  je         0x46121b

// block 00461217
00461217  mov        ecx, esi
00461219  call       eax

// block 0046121b
0046121b  push       edi
0046121c  mov        eax, esi
0046121e  call       0x45c900

// block 00461223
00461223  mov        esi, dword ptr [esi + 0x1c]
00461226  test       esi, esi
00461228  jne        0x461201

// block 0046122a
0046122a  test       dword ptr [0x4cee78], 0x8000
00461234  pop        esi
00461235  je         0x461248

// block 00461237
00461237  push       0x4cf1d0
0046123c  call       dword ptr [0x49808c]

// block 00461242
00461242  dec        byte ptr [0x4cf221]

// block 00461248
00461248  mov        eax, 1
0046124d  ret        
