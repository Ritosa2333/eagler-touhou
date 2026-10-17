004bdb80 55                       push       ebp
004bdb81 8bec                     mov        ebp, esp
004bdb83 83ec0c                   sub        esp, 0xc
004bdb86 894dfc                   mov        dword ptr [ebp - 4], ecx
004bdb89 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bdb8c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004bdb8f 8988ec010000             mov        dword ptr [eax + 0x1ec], ecx
004bdb95 c745f8ffc99a3b           mov        dword ptr [ebp - 8], 0x3b9ac9ff
004bdb9c c745f400000000           mov        dword ptr [ebp - 0xc], 0
004bdba3 8d55f8                   lea        edx, [ebp - 8]
004bdba6 52                       push       edx
004bdba7 8d45f4                   lea        eax, [ebp - 0xc]
004bdbaa 50                       push       eax
004bdbab 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bdbae 81c1ec010000             add        ecx, 0x1ec
004bdbb4 51                       push       ecx
004bdbb5 e8b635fbff               call       0x471170
004bdbba 83c40c                   add        esp, 0xc
004bdbbd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bdbc0 8b00                     mov        eax, dword ptr [eax]
004bdbc2 8982ec010000             mov        dword ptr [edx + 0x1ec], eax
004bdbc8 8be5                     mov        esp, ebp
004bdbca 5d                       pop        ebp
004bdbcb c20400                   ret        4
004bdbce cc                       int3       
004bdbcf cc                       int3       
