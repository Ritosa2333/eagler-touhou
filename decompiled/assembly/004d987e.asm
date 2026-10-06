
// block 004d987e
004d987e  mov        eax, dword ptr [0x7007e454]
004d9883  test       byte ptr cs:[ebx], dh
004d9886  xchg       ebx, eax
004d9887  pop        esp
004d9888  mov        ah, byte ptr [ecx + edx - 0x1f]
004d988c  and        dword ptr [ebx + 0x26a1ac2], ebp
004d9892  cmp        byte ptr [ebp - 0x4add6695], ah
004d9898  lea        edx, [edx + 0x11]
004d989b  jb         0x4d98af

// block 004d98af
004d98af  pop        esp
004d98b0  ret        0x2689
