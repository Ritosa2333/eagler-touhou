0047a340 55                       push       ebp
0047a341 8bec                     mov        ebp, esp
0047a343 83ec08                   sub        esp, 8
0047a346 894df8                   mov        dword ptr [ebp - 8], ecx
0047a349 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047a34c e87f2bfdff               call       0x44ced0
0047a351 85c0                     test       eax, eax
0047a353 7409                     je         0x47a35e
0047a355 c745fc01000000           mov        dword ptr [ebp - 4], 1
0047a35c eb07                     jmp        0x47a365
0047a35e c745fc00000000           mov        dword ptr [ebp - 4], 0
0047a365 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0047a369 8be5                     mov        esp, ebp
0047a36b 5d                       pop        ebp
0047a36c c3                       ret        
0047a36d cc                       int3       
0047a36e cc                       int3       
0047a36f cc                       int3       
