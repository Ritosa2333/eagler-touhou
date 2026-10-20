
// block 0046cec0
0046cec0  push       0x14
0046cec2  push       0x4aaa38
0046cec7  call       0x46fcec

// block 0046cecc
0046cecc  and        dword ptr [ebp - 4], 0

// block 0046ced0
0046ced0  dec        dword ptr [ebp + 0x10]
0046ced3  js         0x46cf0f

// block 0046ced5
0046ced5  mov        ecx, dword ptr [ebp + 8]
0046ced8  sub        ecx, dword ptr [ebp + 0xc]
0046cedb  mov        dword ptr [ebp + 8], ecx
0046cede  call       dword ptr [ebp + 0x14]

// block 0046cee1
0046cee1  jmp        0x46ced0

// block 0046cf0f
0046cf0f  mov        dword ptr [ebp - 4], 0xfffffffe
0046cf16  call       0x46fd31

// block 0046cf1b
0046cf1b  ret        0x10
