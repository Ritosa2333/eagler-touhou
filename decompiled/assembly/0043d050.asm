
// block 0043d050
0043d050  push       -1
0043d052  push       0x49703b
0043d057  mov        eax, dword ptr fs:[0]
0043d05d  push       eax
0043d05e  push       ecx
0043d05f  push       ebx
0043d060  mov        eax, dword ptr [0x4ad138]
0043d065  xor        eax, esp
0043d067  push       eax
0043d068  lea        eax, [esp + 0xc]
0043d06c  mov        dword ptr fs:[0], eax
0043d072  push       0x1edfc
0043d077  call       0x46c9ea

// block 0043d07c
0043d07c  add        esp, 4
0043d07f  mov        dword ptr [esp + 8], eax
0043d083  mov        dword ptr [esp + 0x14], 0
0043d08b  test       eax, eax
0043d08d  je         0x43d098

// block 0043d08f
0043d08f  mov        ebx, eax
0043d091  call       0x43cf90

// block 0043d096
0043d096  jmp        0x43d09a

// block 0043d098
0043d098  xor        eax, eax

// block 0043d09a
0043d09a  mov        dword ptr [0x4b451c], eax
0043d09f  mov        ecx, dword ptr [esp + 0xc]
0043d0a3  mov        dword ptr fs:[0], ecx
0043d0aa  pop        ecx
0043d0ab  pop        ebx
0043d0ac  add        esp, 0x10
0043d0af  ret        
