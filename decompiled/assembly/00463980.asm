
// block 00463980
00463980  sub        esp, 0x104
00463986  mov        eax, dword ptr [0x4ad138]
0046398b  xor        eax, esp
0046398d  mov        dword ptr [esp + 0x100], eax
00463994  lea        eax, [esp]
00463997  push       eax
00463998  call       dword ptr [0x498244]

// block 0046399e
0046399e  xor        eax, eax

// block 004639a0
004639a0  and        byte ptr [esp + eax], 0x7f
004639a4  inc        eax
004639a5  cmp        eax, 0x100
004639aa  jl         0x4639a0

// block 004639ac
004639ac  lea        ecx, [esp]
004639af  push       ecx
004639b0  call       dword ptr [0x498218]

// block 004639b6
004639b6  mov        ecx, dword ptr [esp + 0x100]
004639bd  xor        ecx, esp
004639bf  call       0x46c932

// block 004639c4
004639c4  add        esp, 0x104
004639ca  ret        
