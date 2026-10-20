
// block 00493880
00493880  cmp        dword ptr [0x4d52d4], 0
00493887  je         0x49390f

// block 0049388d
0049388d  sub        esp, 8
00493890  stmxcsr    dword ptr [esp + 4]
00493895  mov        eax, dword ptr [esp + 4]
00493899  and        eax, 0x1f80
0049389e  cmp        eax, 0x1f80
004938a3  jne        0x4938b4

// block 004938a5
004938a5  fnstcw     word ptr [esp]
004938a8  mov        ax, word ptr [esp]
004938ac  and        ax, 0x7f
004938b0  cmp        ax, 0x7f

// block 004938b4
004938b4  lea        esp, [esp + 8]
004938b8  jne        0x49390f

// block 004938ba
004938ba  jmp        0x495e08

// block 0049390f
0049390f  lea        edx, [esp + 4]
00493913  call       0x4956a5

// block 00495e08
00495e08  movlpd     xmm0, qword ptr [esp + 4]
