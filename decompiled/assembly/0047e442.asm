
// block 0047e442
0047e442  mov        edi, edi
0047e444  push       ebp
0047e445  mov        ebp, esp
0047e447  cmp        dword ptr [ecx + 4], 1
0047e44b  jne        0x47e464

// block 0047e44d
0047e44d  push       4
0047e44f  push       0x49ddf8
0047e454  push       dword ptr [ebp + 0xc]
0047e457  push       dword ptr [ebp + 8]
0047e45a  call       0x47e144

// block 0047e45f
0047e45f  add        esp, 0x10
0047e462  jmp        0x47e467

// block 0047e464
0047e464  mov        eax, dword ptr [ebp + 8]

// block 0047e467
0047e467  pop        ebp
0047e468  ret        8
