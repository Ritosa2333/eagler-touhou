
// block 004db2f3
004db2f3  inc        ecx
004db2f4  sub        dword ptr [ebp + 0x200e21b0], 0
004db2fb  out        dx, al
004db2fc  out        dx, eax
004db2fd  test       al, ch
004db2ff  jecxz      0x4db2d6

// block 004db301
004db301  and        byte ptr [edx + 0x42], bl
004db304  push       edx
004db305  xchg       ebp, eax
