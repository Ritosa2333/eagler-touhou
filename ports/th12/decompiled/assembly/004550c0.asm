
// block 004550c0
004550c0  sub        esp, 0x10c
004550c6  mov        eax, dword ptr [0x4ad138]
004550cb  xor        eax, esp
004550cd  mov        dword ptr [esp + 0x108], eax
004550d4  mov        eax, dword ptr [esp + 0x110]
004550db  push       eax
004550dc  lea        eax, [esp + 8]
004550e0  push       0x4a0f40
004550e5  push       eax
004550e6  call       0x46cbbb

// block 004550eb
004550eb  lea        eax, [esp + 0x10]
004550ef  add        esp, 0xc
004550f2  lea        edx, [eax + 1]

// block 004550f5
004550f5  mov        cl, byte ptr [eax]
004550f7  inc        eax
004550f8  test       cl, cl
004550fa  jne        0x4550f5

// block 004550fc
004550fc  sub        eax, edx
004550fe  lea        eax, [esp + eax + 4]
00455102  lea        ecx, [esp + 4]
00455106  push       ecx
00455107  mov        byte ptr [eax - 3], 0x6d
0045510b  mov        byte ptr [eax - 2], 0x61
0045510f  mov        byte ptr [eax - 1], 0x70
00455113  call       0x463df0

// block 00455118
00455118  test       eax, eax
0045511a  je         0x455181

// block 0045511c
0045511c  push       1
0045511e  lea        edx, [esp + 4]
00455122  push       edx
00455123  lea        eax, [esp + 0xc]
00455127  call       0x463c10

// block 0045512c
0045512c  mov        ecx, dword ptr [esi + 0x110]
00455132  add        ecx, dword ptr [esi + 0x114]
00455138  mov        dword ptr [esi + 0x134], eax
0045513e  xor        eax, eax
00455140  test       ecx, ecx
00455142  jle        0x45518b

// block 00455144
00455144  jmp        0x455150

// block 00455150
00455150  mov        ecx, dword ptr [esi + 0x134]
00455156  add        dword ptr [ecx + eax*4], ecx
00455159  mov        edx, dword ptr [esi + 0x110]
0045515f  add        edx, dword ptr [esi + 0x114]
00455165  inc        eax
00455166  cmp        eax, edx
00455168  jl         0x455150

// block 0045516a
0045516a  mov        ecx, dword ptr [esp + 0x108]
00455171  xor        ecx, esp
00455173  call       0x46c932

// block 00455178
00455178  add        esp, 0x10c
0045517e  ret        4

// block 00455181
00455181  mov        dword ptr [esi + 0x134], 0

// block 0045518b
0045518b  mov        ecx, dword ptr [esp + 0x108]
00455192  xor        ecx, esp
00455194  call       0x46c932

// block 00455199
00455199  add        esp, 0x10c
0045519f  ret        4
