004678c0 55                       push       ebp
004678c1 8bec                     mov        ebp, esp
004678c3 83ec24                   sub        esp, 0x24
004678c6 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004678c9 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004678cc f30f1000                 movss      xmm0, dword ptr [eax]
004678d0 f30f5901                 mulss      xmm0, dword ptr [ecx]
004678d4 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004678d7 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004678da f30f104a04               movss      xmm1, dword ptr [edx + 4]
004678df f30f594804               mulss      xmm1, dword ptr [eax + 4]
004678e4 f30f58c1                 addss      xmm0, xmm1
004678e8 51                       push       ecx
004678e9 f30f110424               movss      dword ptr [esp], xmm0
004678ee e83df2fdff               call       0x446b30
004678f3 83c404                   add        esp, 4
004678f6 d95dfc                   fstp       dword ptr [ebp - 4]
004678f9 51                       push       ecx
004678fa f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004678ff f30f110424               movss      dword ptr [esp], xmm0
00467904 e877ddfdff               call       0x445680
00467909 83c404                   add        esp, 4
0046790c d95df8                   fstp       dword ptr [ebp - 8]
0046790f f30f1005a0e15600         movss      xmm0, dword ptr [0x56e1a0]
00467917 0f2f45f8                 comiss     xmm0, dword ptr [ebp - 8]
0046791b 7626                     jbe        0x467943
0046791d 51                       push       ecx
0046791e f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00467923 f30f110424               movss      dword ptr [esp], xmm0
00467928 8d4dec                   lea        ecx, [ebp - 0x14]
0046792b 51                       push       ecx
0046792c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0046792f e88c1afcff               call       0x4293c0
00467934 8b10                     mov        edx, dword ptr [eax]
00467936 8b4004                   mov        eax, dword ptr [eax + 4]
00467939 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0046793c 8911                     mov        dword ptr [ecx], edx
0046793e 894104                   mov        dword ptr [ecx + 4], eax
00467941 eb3e                     jmp        0x467981
00467943 51                       push       ecx
00467944 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00467949 f30f110424               movss      dword ptr [esp], xmm0
0046794e 8d55e4                   lea        edx, [ebp - 0x1c]
00467951 52                       push       edx
00467952 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00467955 e856faffff               call       0x4673b0
0046795a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0046795d 51                       push       ecx
0046795e f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00467963 f30f110424               movss      dword ptr [esp], xmm0
00467968 8d45dc                   lea        eax, [ebp - 0x24]
0046796b 50                       push       eax
0046796c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046796f e84c1afcff               call       0x4293c0
00467974 8b08                     mov        ecx, dword ptr [eax]
00467976 8b5004                   mov        edx, dword ptr [eax + 4]
00467979 8b4508                   mov        eax, dword ptr [ebp + 8]
0046797c 8908                     mov        dword ptr [eax], ecx
0046797e 895004                   mov        dword ptr [eax + 4], edx
00467981 8be5                     mov        esp, ebp
00467983 5d                       pop        ebp
00467984 c3                       ret        
00467985 cc                       int3       
00467986 cc                       int3       
00467987 cc                       int3       
00467988 cc                       int3       
00467989 cc                       int3       
0046798a cc                       int3       
0046798b cc                       int3       
0046798c cc                       int3       
0046798d cc                       int3       
0046798e cc                       int3       
0046798f cc                       int3       
