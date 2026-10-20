
// block 004939b0
004939b0  cmp        dword ptr [0x4d52d4], 0
004939b7  je         0x493a3f

// block 004939bd
004939bd  sub        esp, 8
004939c0  stmxcsr    dword ptr [esp + 4]
004939c5  mov        eax, dword ptr [esp + 4]
004939c9  and        eax, 0x1f80
004939ce  cmp        eax, 0x1f80
004939d3  jne        0x4939e4

// block 004939d5
004939d5  fnstcw     word ptr [esp]
004939d8  mov        ax, word ptr [esp]
004939dc  and        ax, 0x7f
004939e0  cmp        ax, 0x7f

// block 004939e4
004939e4  lea        esp, [esp + 8]
004939e8  jne        0x493a3f

// block 004939ea
004939ea  jmp        0x495fd8

// block 00493a3f
00493a3f  lea        edx, [esp + 4]
00493a43  call       0x4956a5

// block 00495fd8
00495fd8  movlpd     xmm0, qword ptr [esp + 4]
