
// block 0048061b
0048061b  mov        edi, edi
0048061d  push       ebp
0048061e  mov        ebp, esp
00480620  mov        eax, dword ptr [0x4b4318]
00480625  cmp        byte ptr [eax], 0x3f
00480628  jne        0x480651

// block 0048062a
0048062a  inc        eax
0048062b  cmp        byte ptr [eax], 0x24
0048062e  jne        0x48063e

// block 00480630
00480630  push       1
00480632  push       dword ptr [ebp + 8]
00480635  call       0x4800f3

// block 0048063a
0048063a  pop        ecx
0048063b  pop        ecx
0048063c  jmp        0x480660

// block 0048063e
0048063e  push       0
00480640  push       0
00480642  push       dword ptr [ebp + 8]
00480645  mov        dword ptr [0x4b4318], eax
0048064a  call       0x47fb56

// block 0048064f
0048064f  jmp        0x48065d

// block 00480651
00480651  push       0
00480653  push       1
00480655  push       dword ptr [ebp + 8]
00480658  call       0x48023a

// block 0048065d
0048065d  add        esp, 0xc

// block 00480660
00480660  mov        eax, dword ptr [ebp + 8]
00480663  pop        ebp
00480664  ret        
