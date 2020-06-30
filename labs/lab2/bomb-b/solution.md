## Phase 1

解题过程：

```
gdb bomb
layout split # 看汇编代码和部分源码
b 73         # 源代码 73 行位置加 breakpoint
r            # 运行 bomb
n            # 运行下一行源代码
             # 随便输入个字符串
stepi        # 运行下一行汇编代码，直到运行到 phase_1 的汇编代码
```

这边截取两段可以给出提示的汇编代码：

```
0x400e32 <main+146>     callq  0x40149e <read_line>
0x400e37 <main+151>     mov    %rax,%rdi
0x400e3a <main+154>     callq  0x400ee0 <phase_1>
```

这里我们可以看到 %rax 中存的是 read_line 方法的返回值，然后再被存到 %rdi 中，也就是方法的第一个参数，传递并 call phase_1 方法。

```
0x400ee0 <phase_1>      sub    $0x8,%rsp
0x400ee4 <phase_1+4>    mov    $0x402400,%esi
0x400ee9 <phase_1+9>    callq  0x401338 <strings_not_equal>
0x400eee <phase_1+14>   test   %eax,%eax
0x400ef0 <phase_1+16>   je     0x400ef7 <phase_1+23>
0x400ef2 <phase_1+18>   callq  0x40143a <explode_bomb>
```

这里关键的是 strings_not_equal 方法，返回值如果是非零（也就是真）的话，就会触发 explode_bomb。

call strings_not_equal 方法之前的一句是把立即数 0x402400 存入 %esi（也就是方法的第二个参数），这里可以推论 strings_not_equal 有两个输入参数，第一个是前面输入的字符串的指针，第二个是待比较的字符串的指针。所以我们尝试看一下指针 0x402400 中存的是什么：

```
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

这样就得到了第一个字符串：Border relations with Canada have never been better.

## Phase 2

```
b 82         # 源代码 82 行位置加 breakpoint
r            # 运行 bomb
             # 过掉 phase 1，随便输入 phase 2 的字符串
stepi        # 运行下一行汇编代码，直到运行到 phase_2 的汇编代码
```

我们得到以下汇编代码：

```
0x400efc <phase_2>      push   %rbp         # 保存 %rbp
0x400efd <phase_2+1>    push   %rbx         # 保存 %rbx
0x400efe <phase_2+2>    sub    $0x28,%rsp   # 栈指针下移 40 字节
0x400f02 <phase_2+6>    mov    %rsp,%rsi    # 把栈指针作为第二个参数
0x400f05 <phase_2+9>    callq  0x40145c <read_six_numbers>
0x400f0a <phase_2+14>   cmpl   $0x1,(%rsp)  # 判断第一个数字是否等于 1，如果不等于就引爆炸弹
0x400f0e <phase_2+18>   je     0x400f30 <phase_2+52>
0x400f10 <phase_2+20>   callq  0x40143a <explode_bomb>
0x400f15 <phase_2+25>   jmp    0x400f30 <phase_2+52>
0x400f17 <phase_2+27>   mov    -0x4(%rbx),%eax
0x400f1a <phase_2+30>   add    %eax,%eax
0x400f1c <phase_2+32>   cmp    %eax,(%rbx)        # 从以上三句可以推断，当前数字应该是前一个数字的两倍
0x400f1e <phase_2+34>   je     0x400f25 <phase_2+41>
0x400f20 <phase_2+36>   callq  0x40143a <explode_bomb>
0x400f25 <phase_2+41>   add    $0x4,%rbx          # 指针指向下个数字
0x400f29 <phase_2+45>   cmp    %rbp,%rbx          # 检查边界
0x400f2c <phase_2+48>   jne    0x400f17 <phase_2+27>
0x400f2e <phase_2+50>   jmp    0x400f3c <phase_2+64>
0x400f30 <phase_2+52>   lea    0x4(%rsp),%rbx     # 把当前指针指向第二个数字
0x400f35 <phase_2+57>   lea    0x18(%rsp),%rbp    # 根据后面我们可以推测这里保存的是边界，也就是栈顶 + 24 的位置
0x400f3a <phase_2+62>   jmp    0x400f17 <phase_2+27>
0x400f3c <phase_2+64>   add    $0x28,%rsp         # 恢复栈指针
0x400f40 <phase_2+68>   pop    %rbx
0x400f41 <phase_2+69>   pop    %rbp
0x400f42 <phase_2+70>   retq
```

```
0x40145c <read_six_numbers>     sub    $0x18,%rsp       # 栈指针下移 24 字节
0x401460 <read_six_numbers+4>   mov    %rsi,%rdx        # 把 phase 2 的栈顶移到 %rdx 中，作为第三个方法参数
0x401463 <read_six_numbers+7>   lea    0x4(%rsi),%rcx   # 把 phase 2 栈顶 + 4 的指针移到 %rcx 中，作为第四个方法参数
0x401467 <read_six_numbers+11>  lea    0x14(%rsi),%rax
0x40146b <read_six_numbers+15>  mov    %rax,0x8(%rsp)   # 把 phase 2 栈顶 + 20 这个指针移到当前栈顶 + 8 的位置
0x401470 <read_six_numbers+20>  lea    0x10(%rsi),%rax
0x401474 <read_six_numbers+24>  mov    %rax,(%rsp)      # 把 phase 2 栈顶 + 16 这个指针移到当前栈顶的位置
0x401478 <read_six_numbers+28>  lea    0xc(%rsi),%r9    # phase 2 栈顶 + 12 的指针作为第六个方法参数
0x40147c <read_six_numbers+32>  lea    0x8(%rsi),%r8    # phase 2 栈顶 + 8 的指针作为第五个方法参数
0x401480 <read_six_numbers+36>  mov    $0x4025c3,%esi   # 把立即数 0x4025c3 作为第二个方法参数
0x401485 <read_six_numbers+41>  mov    $0x0,%eax        # 返回值设为 0
0x40148a <read_six_numbers+46>  callq  0x400bf0 <__isoc99_sscanf@plt>
0x40148f <read_six_numbers+51>  cmp    $0x5,%eax        # 判断 sscanf 的返回值是否大于 5
0x401492 <read_six_numbers+54>  jg     0x401499 <read_six_numbers+61>
0x401494 <read_six_numbers+56>  callq  0x40143a <explode_bomb>
0x401499 <read_six_numbers+61>  add    $0x18,%rsp
0x40149d <read_six_numbers+65>  retq
```

查看立即数 0x4025c3 指向的内容，发现是个输入格式：

```
(gdb) x/s 0x4025c3
0x4025c3:       "%d %d %d %d %d %d"
```

再根据 sscanf 的方法签名，我们可以推测 read_six_numbers 是把输入的字符串按照 "%d %d %d %d %d %d" 的格式移到 phase 2 栈顶 +0, +4, +8, +12，+16 和 +20 的位置（最多六个方法参数，所以最后这两个是在当前的栈上分配的），并且每个数字占 4 个字节，是 int。

由于 sscanf 的返回值是填充的变量的个数，这里判断如果返回值小于或等于 5，就会引爆炸弹。所以到这里我们可以推断我们输入的字符串必定包含 6 个空格分隔的数字，并且我们知道它们被读取后存到了哪里。

然后再回到 phase_2，分析汇编码可以得知，程序检查了当前栈指针 +0 到 +24 之间的六个 int 数字，第一个数字是 1，之后每个数字是前一个数字的 2 倍。所以答案是: 1 2 4 8 16 32

## Phase 3

```
0x400f43 <phase_3>      sub    $0x18,%rsp
0x400f47 <phase_3+4>    lea    0xc(%rsp),%rcx           # sscanf 的第二个数字存在栈指针 +12 的位置
0x400f4c <phase_3+9>    lea    0x8(%rsp),%rdx           # sscanf 的第一个数字存在栈指针 +8 的位置
0x400f51 <phase_3+14>   mov    $0x4025cf,%esi           # x/s 0x4025cf 的结果是 "%d %d"
0x400f56 <phase_3+19>   mov    $0x0,%eax
0x400f5b <phase_3+24>   callq  0x400bf0 <__isoc99_sscanf@plt>
0x400f60 <phase_3+29>   cmp    $0x1,%eax                # scan 得到的数字数目必须大于 1
0x400f63 <phase_3+32>   jg     0x400f6a <phase_3+39>
0x400f65 <phase_3+34>   callq  0x40143a <explode_bomb>
0x400f6a <phase_3+39>   cmpl   $0x7,0x8(%rsp)           # 比较栈指针 +8 位置存储的数字和立即数 7
0x400f6f <phase_3+44>   ja     0x400fad <phase_3+106>   # 如果大于就转移（无符号比较），继而引爆炸弹，所以这里可以推测这个数字小于等于 7
0x400f71 <phase_3+46>   mov    0x8(%rsp),%eax           # 以下我们假设第一个数字为 x
0x400f75 <phase_3+50>   jmpq   *0x402470(,%rax,8)       # 间接跳转到 0x402470 + 8x，这个地方我们要看下 0x402470 中存的是什么，见后面的列表
0x400f7c <phase_3+57>   mov    $0xcf,%eax               # 207
0x400f81 <phase_3+62>   jmp    0x400fbe <phase_3+123>
0x400f83 <phase_3+64>   mov    $0x2c3,%eax              # 707
0x400f88 <phase_3+69>   jmp    0x400fbe <phase_3+123>
0x400f8a <phase_3+71>   mov    $0x100,%eax              # 256
0x400f8f <phase_3+76>   jmp    0x400fbe <phase_3+123>
0x400f91 <phase_3+78>   mov    $0x185,%eax              # 389
0x400f96 <phase_3+83>   jmp    0x400fbe <phase_3+123>
0x400f98 <phase_3+85>   mov    $0xce,%eax               # 206
0x400f9d <phase_3+90>   jmp    0x400fbe <phase_3+123>
0x400f9f <phase_3+92>   mov    $0x2aa,%eax              # 682
0x400fa4 <phase_3+97>   jmp    0x400fbe <phase_3+123>
0x400fa6 <phase_3+99>   mov    $0x147,%eax              # 327
0x400fab <phase_3+104>  jmp    0x400fbe <phase_3+123>
0x400fad <phase_3+106>  callq  0x40143a <explode_bomb>
0x400fb2 <phase_3+111>  mov    $0x0,%eax
0x400fb7 <phase_3+116>  jmp    0x400fbe <phase_3+123>
0x400fb9 <phase_3+118>  mov    $0x137,%eax              # 311
0x400fbe <phase_3+123>  cmp    0xc(%rsp),%eax           # 比较第二个数字与 %eax
0x400fc2 <phase_3+127>  je     0x400fc9 <phase_3+134>   # 如果以上比较相等，则过关
0x400fc4 <phase_3+129>  callq  0x40143a <explode_bomb>
0x400fc9 <phase_3+134>  add    $0x18,%rsp
0x400fcd <phase_3+138>  retq
```

我们可以检查得出，0x402470 以及其偏移的若干地址，都存着跳转地址，每个跳转地址都对应一个待比较的立即数。

```
(gdb) x/x 0x402470
0x402470:       0x00400f7c
(gdb) x/x 0x402478
0x402478:       0x00400fb9
(gdb) x/x 0x402480
0x402480:       0x00400f83
(gdb) x/x 0x402488
0x402488:       0x00400f8a
(gdb) x/x 0x402490
0x402490:       0x00400f91
(gdb) x/x 0x402498
0x402498:       0x00400f98
(gdb) x/x 0x4024a0
0x4024a0:       0x00400f9f
(gdb) x/x 0x4024a8
0x4024a8:       0x00400fa6
```

由上面的内容我们可以得出 phase 3 有多组解：

```
0 207
1 311
2 707
3 256
4 389
5 206
6 682
7 327
```

## Phase 4

```
0x40100c <phase_4>      sub    $0x18,%rsp
0x401010 <phase_4+4>    lea    0xc(%rsp),%rcx            # 存第二个数字
0x401015 <phase_4+9>    lea    0x8(%rsp),%rdx            # 存第一个数字
0x40101a <phase_4+14>   mov    $0x4025cf,%esi            # "%d %d"
0x40101f <phase_4+19>   mov    $0x0,%eax
0x401024 <phase_4+24>   callq  0x400bf0 <__isoc99_sscanf@plt>
0x401029 <phase_4+29>   cmp    $0x2,%eax
0x40102c <phase_4+32>   jne    0x401035 <phase_4+41>     # 必须是两个数字
0x40102e <phase_4+34>   cmpl   $0xe,0x8(%rsp)
0x401033 <phase_4+39>   jbe    0x40103a <phase_4+46>     # 第一个数字必须 <= 14, >= 0（无符号）
0x401035 <phase_4+41>   callq  0x40143a <explode_bomb>
0x40103a <phase_4+46>   mov    $0xe,%edx                 # func4 第三个参数为 14
0x40103f <phase_4+51>   mov    $0x0,%esi                 # 第二个参数为 0
0x401044 <phase_4+56>   mov    0x8(%rsp),%edi            # 第一个参数为我们输入的第一个数字
0x401048 <phase_4+60>   callq  0x400fce <func4>
0x40104d <phase_4+65>   test   %eax,%eax
0x40104f <phase_4+67>   jne    0x401058 <phase_4+76>     # 测试 func4 的返回值，如果不为零，则引爆炸弹
0x401051 <phase_4+69>   cmpl   $0x0,0xc(%rsp)
0x401056 <phase_4+74>   je     0x40105d <phase_4+81>     # 测试我们输入的第二个数字，如果为 0，则过关
0x401058 <phase_4+76>   callq  0x40143a <explode_bomb>
0x40105d <phase_4+81>   add    $0x18,%rsp
0x401061 <phase_4+85>   retq
```

从上面汇编码可以看出，输入为两个数字，第二个数字必须为 0，第一个数字要使得 func4 返回值为 0，因此我们接下来分析 func4。

```
0000000000400fce <func4>:
  400fce:   48 83 ec 08             sub    $0x8,%rsp
  400fd2:   89 d0                   mov    %edx,%eax
  400fd4:   29 f0                   sub    %esi,%eax              # eax = j - i
  400fd6:   89 c1                   mov    %eax,%ecx
  400fd8:   c1 e9 1f                shr    $0x1f,%ecx
  400fdb:   01 c8                   add    %ecx,%eax              # 以上几句是在判断 eax 的符号
  400fdd:   d1 f8                   sar    %eax                   # eax /= 2
  400fdf:   8d 0c 30                lea    (%rax,%rsi,1),%ecx     # val = eax + i = (j + i) / 2
  400fe2:   39 f9                   cmp    %edi,%ecx              # 比较输入和 val
  400fe4:   7e 0c                   jle    400ff2 <func4+0x24>    # 如果输入小于或等于 val，则跳转
  400fe6:   8d 51 ff                lea    -0x1(%rcx),%edx
  400fe9:   e8 e0 ff ff ff          callq  400fce <func4>
  400fee:   01 c0                   add    %eax,%eax
  400ff0:   eb 15                   jmp    401007 <func4+0x39>
  400ff2:   b8 00 00 00 00          mov    $0x0,%eax              # eax 为零
  400ff7:   39 f9                   cmp    %edi,%ecx
  400ff9:   7d 0c                   jge    401007 <func4+0x39>    # 如果输入大于等于 val，则 return
  400ffb:   8d 71 01                lea    0x1(%rcx),%esi         # 否则递归 call，第二个参数变为 val + 1
  400ffe:   e8 cb ff ff ff          callq  400fce <func4>
  401003:   8d 44 00 01             lea    0x1(%rax,%rax,1),%eax
  401007:   48 83 c4 08             add    $0x8,%rsp
  40100b:   c3                      retq
```

func4 开头几句看似很绕的汇编实际是在计算 (j - i) / 2，因为用右移来实现除法的话，要注意负数无法被整除时的舍入问题，所以这段代码中的计算与符号有关。func4 逆向工程为 C 代码为：

```
int func4(int n, int i, int j) {
    int val = (j + i) / 2;
    if (val <= n) {
        if (val >= n) return 0;
        else return 2 * func4(n, val+1, j) + 1;
    } else {
        return 2 * func4(n, i, val-1);
    }
}
```

这段代码的作用类似二分搜索，但是目标值只能向小的半段搜索，否则 `return 2 * func4(n, val+1, j) + 1;` 会返回非零的值。我们分析这个函数的取值可以得知满足条件的数有 0, 1, 3, 7

所以 phase 4 有四组可能的答案：

```
0 0
1 0
3 0
7 0
```

## Phase 5
```
0000000000401062 <phase_5>:
  401062: 53                    push   %rbx
  401063: 48 83 ec 20           sub    $0x20,%rsp
  401067: 48 89 fb              mov    %rdi,%rbx
  40106a: 64 48 8b 04 25 28 00  mov    %fs:0x28,%rax
  401071: 00 00 
  401073: 48 89 44 24 18        mov    %rax,0x18(%rsp)
  401078: 31 c0                 xor    %eax,%eax
  40107a: e8 9c 02 00 00        callq  40131b <string_length>
  40107f: 83 f8 06              cmp    $0x6,%eax
  401082: 74 4e                 je     4010d2 <phase_5+0x70>
  401084: e8 b1 03 00 00        callq  40143a <explode_bomb>
  401089: eb 47                 jmp    4010d2 <phase_5+0x70>
  40108b: 0f b6 0c 03           movzbl (%rbx,%rax,1),%ecx
  40108f: 88 0c 24              mov    %cl,(%rsp)
  401092: 48 8b 14 24           mov    (%rsp),%rdx
  401096: 83 e2 0f              and    $0xf,%edx
  401099: 0f b6 92 b0 24 40 00  movzbl 0x4024b0(%rdx),%edx
  4010a0: 88 54 04 10           mov    %dl,0x10(%rsp,%rax,1)
  4010a4: 48 83 c0 01           add    $0x1,%rax
  4010a8: 48 83 f8 06           cmp    $0x6,%rax
  4010ac: 75 dd                 jne    40108b <phase_5+0x29>
  4010ae: c6 44 24 16 00        movb   $0x0,0x16(%rsp)
  4010b3: be 5e 24 40 00        mov    $0x40245e,%esi
  4010b8: 48 8d 7c 24 10        lea    0x10(%rsp),%rdi
  4010bd: e8 76 02 00 00        callq  401338 <strings_not_equal>
  4010c2: 85 c0                 test   %eax,%eax
  4010c4: 74 13                 je     4010d9 <phase_5+0x77>
  4010c6: e8 6f 03 00 00        callq  40143a <explode_bomb>
  4010cb: 0f 1f 44 00 00        nopl   0x0(%rax,%rax,1)
  4010d0: eb 07                 jmp    4010d9 <phase_5+0x77>
  4010d2: b8 00 00 00 00        mov    $0x0,%eax
  4010d7: eb b2                 jmp    40108b <phase_5+0x29>
  4010d9: 48 8b 44 24 18        mov    0x18(%rsp),%rax
  4010de: 64 48 33 04 25 28 00  xor    %fs:0x28,%rax
  4010e5: 00 00 
  4010e7: 74 05                 je     4010ee <phase_5+0x8c>
  4010e9: e8 42 fa ff ff        callq  400b30 <__stack_chk_fail@plt>
  4010ee: 48 83 c4 20           add    $0x20,%rsp
  4010f2: 5b                    pop    %rbx
  4010f3: c3                    retq   
```

根据x86-64寄存器使用规范，%rdi寄存器存储的是第一个参数的值，由于输入的是字符串，因此%rdi存储的应该是输入字符串的起始地址。0x401067处的指令mov %rdi,%rbx将字符串起始地址保存在%rbx中，即%rbx为基址寄存器。指令xor %eax,%eax的作用是将%eax清零，接着调用string_length函数获取输入字符串的长度，并将长度值（返回值）存储于%eax。指令cmp $0x6,%eax将string_length的返回值与常数6作比较，若不相等则会引爆炸弹，由此可以得知，phase_5的输入字符串长度应该等于6。

当输入字符串的长度等于6，phase_5跳转至0x4010d2处执行指令mov $0x0,%eax，接着又跳转至0x40108b处执行指令movzbl (%rbx,%rax,1),%ecx，可以发现0x40108b至0x4010ac构成了一个循环，且在循环退出后在0x4010bd处调动了strings_not_equal来比较字符串是否相等，若相等，则拆弹成功。其中，由mov $0x40245e,%esi指令可知，待比较的字符串存储于地址0x40245e处，打印以该地址作为起始地址的字符串

```
(gdb) x/s 0x40245e
0x40245e: "flyers"
```
待比较的字符串为flyers，且长度也为6。所以，接下来的关键任务是需要对循环操作进行分析，理解该循环操作对输入字符串做了哪些操作。提取循环操作的代码
```
40108b: 0f b6 0c 03           movzbl (%rbx,%rax,1),%ecx
  40108f: 88 0c 24              mov    %cl,(%rsp)
  401092: 48 8b 14 24           mov    (%rsp),%rdx
  401096: 83 e2 0f              and    $0xf,%edx
  401099: 0f b6 92 b0 24 40 00  movzbl 0x4024b0(%rdx),%edx
  4010a0: 88 54 04 10           mov    %dl,0x10(%rsp,%rax,1)
  4010a4: 48 83 c0 01           add    $0x1,%rax
  4010a8: 48 83 f8 06           cmp    $0x6,%rax
  4010ac: 75 dd                 jne    40108b <phase_5+0x29>
```

由于%rbx存储的是输入字符串的起始地址，%rax初始化为0，其作用等价于下标，因此movzbl (%rbx,%rax,1),%ecx指令的作用是将字符串的第%rax个字符存储于%ecx，movzbl意味做了零扩展。接着，mov %cl,(%rsp)指令取%ecx的低8位，即一个字符的大小，通过内存间接存储至%rdx中。and $0xf,%edx指令将%edx的值与常数0xf进行位与，由指令movzbl 0x4024b0(%rdx),%edx可知，位与后的值将会作为偏移量，以0x4024b0为基址，将偏移后的值存储至%edx。最后，指令mov %dl,0x10(%rsp,%rax,1)以%edx低8位的值作为新的字符，对原有字符进行替换。

综上，phase_5遍历输入字符串的每个字符，将字符的低4位作为偏移量，以0x4024b0为起始地址，将新地址对应的字符替换原有字符，最终得到flyers字符串。打印0x4024b0处的内容，

```
(gdb) x/s 0x4024b0
0x4024b0 <array.3449>: "maduiersnfotvbySo you think you can stop the bomb with ctrl-c, do you?"
```
例如，如果要得到字符f，那么偏移量应为9，二进制表示为1001，通过查找ASCII表，可知字符i的ASCII编码为01101001，满足要求。剩余5个字符采用同样的策略可以依次求得，最终，phase_5的输入字符串为ionefg。

## Phase 6
```
00000000004010f4 <phase_6>:
  4010f4: 41 56                 push   %r14
  4010f6: 41 55                 push   %r13
  4010f8: 41 54                 push   %r12
  4010fa: 55                    push   %rbp
  4010fb: 53                    push   %rbx
  4010fc: 48 83 ec 50           sub    $0x50,%rsp
  401100: 49 89 e5              mov    %rsp,%r13
  401103: 48 89 e6              mov    %rsp,%rsi
  401106: e8 51 03 00 00        callq  40145c <read_six_numbers>
  40110b: 49 89 e6              mov    %rsp,%r14               # %r14存储数组起始地址
  40110e: 41 bc 00 00 00 00     mov    $0x0,%r12d              # 将%r12d初始化为0
  
  #################### Section 1:确认数组中所有的元素小于等于6且不存在重复值 ###################
  401114: 4c 89 ed              mov    %r13,%rbp               # %r13和%rbp存储数组某个元素的地址，并不是第1个元素，意识到这点需要结合0x40114d处的指令
  401117: 41 8b 45 00           mov    0x0(%r13),%eax          # %eax存储第%r13个数
  40111b: 83 e8 01              sub    $0x1,%eax               # 将%eax的值减1
  40111e: 83 f8 05              cmp    $0x5,%eax               # 将%eax的值与常数5做比较
  401121: 76 05                 jbe    401128 <phase_6+0x34>
  401123: e8 12 03 00 00        callq  40143a <explode_bomb>
  401128: 41 83 c4 01           add    $0x1,%r12d              # 如果%eax的值小于等于5，%r12d加1
  40112c: 41 83 fc 06           cmp    $0x6,%r12d              # 将%r12d与常数6做比较
  401130: 74 21                 je     401153 <phase_6+0x5f>
  401132: 44 89 e3              mov    %r12d,%ebx              # %ebx起了数组下标的作用
  
  # 用于判断数组6个数是否存在重复值，若存在，引爆炸弹
  401135: 48 63 c3              movslq %ebx,%rax               # 将数组下标存储至%rax
  401138: 8b 04 84              mov    (%rsp,%rax,4),%eax      # 将下一个数存储至%eax
  40113b: 39 45 00              cmp    %eax,0x0(%rbp)          # 将第1个数与%eax的值(当前数)做比较
  40113e: 75 05                 jne    401145 <phase_6+0x51>   # 若相等，引爆炸弹   
  401140: e8 f5 02 00 00        callq  40143a <explode_bomb>
  401145: 83 c3 01              add    $0x1,%ebx               # 数组下标加1            
  401148: 83 fb 05              cmp    $0x5,%ebx               # 判断数组下标是否越界(<=5)
  40114b: 7e e8                 jle    401135 <phase_6+0x41>
  
  40114d: 49 83 c5 04           add    $0x4,%r13               # %r13存储数组下一个数的地址
  401151: eb c1                 jmp    401114 <phase_6+0x20>
  ####################################### Section 1 end ######################################
  
  ################ Section 2：用7减去数组的每个元素，并将相减后的元素替换原有元素 #################
  401153: 48 8d 74 24 18        lea    0x18(%rsp),%rsi         # 0x18(%rsp)是数组的边界地址：0x18 = 24         
  401158: 4c 89 f0              mov    %r14,%rax               # 将数组起始地址存储于%rax
  40115b: b9 07 00 00 00        mov    $0x7,%ecx
  
  401160: 89 ca                 mov    %ecx,%edx               # %edx = 7
  401162: 2b 10                 sub    (%rax),%edx             # %edx = 7 - 数组元素
  401164: 89 10                 mov    %edx,(%rax)             # 用相减后的元素(%edx)替换原有元素
  401166: 48 83 c0 04           add    $0x4,%rax               # %rax存储数组下一个元素的地址
  40116a: 48 39 f0              cmp    %rsi,%rax               # 判断是否越界
  40116d: 75 f1                 jne    401160 <phase_6+0x6c>
  ####################################### Section 2 end ######################################
  
  ########################## Section 3：根据输入数组重排结构体数组 ##############################
  40116f: be 00 00 00 00        mov    $0x0,%esi               # 将%esi初始化为0，作为数组下标
  401174: eb 21                 jmp    401197 <phase_6+0xa3>
  
  401176: 48 8b 52 08           mov    0x8(%rdx),%rdx          # 0x8(%rdx)为下一个元素的地址
  40117a: 83 c0 01              add    $0x1,%eax               
  40117d: 39 c8                 cmp    %ecx,%eax               # %ecx存储了数组当前值(第%esi个元素)
  40117f: 75 f5                 jne    401176 <phase_6+0x82>
  
  401181: eb 05                 jmp    401188 <phase_6+0x94>
  401183: ba d0 32 60 00        mov    $0x6032d0,%edx          # %edx存储结构体数组第1个元素的地址
  401188: 48 89 54 74 20        mov    %rdx,0x20(%rsp,%rsi,2)  # %rsi的初始值为0；该指令的作用是将结构体数组的第%ecx个元素的地址存储在内存的某个位置(以%rsp + 0x20为基地址，%rsi为偏移量)
  40118d: 48 83 c6 04           add    $0x4,%rsi               # 增加偏移量
  401191: 48 83 fe 18           cmp    $0x18,%rsi
  401195: 74 14                 je     4011ab <phase_6+0xb7>
  401197: 8b 0c 34              mov    (%rsp,%rsi,1),%ecx      # %ecx存储数组第%esi个元素
  40119a: 83 f9 01              cmp    $0x1,%ecx               # 将数组第%esi个元素与常数1做比较
  40119d: 7e e4                 jle    401183 <phase_6+0x8f>   # 实际上不会小于1，如果数组的第1个元素等于1，那么跳转至0x401183处
  40119f: b8 01 00 00 00        mov    $0x1,%eax
  4011a4: ba d0 32 60 00        mov    $0x6032d0,%edx          # %edx存储结构体数组第1个元素的地址
  4011a9: eb cb                 jmp    401176 <phase_6+0x82>
  ####################################### Section 3 end ######################################
  
  ######################### Section 4：修改结构体数组元素的next域值 #############################
  4011ab: 48 8b 5c 24 20        mov    0x20(%rsp),%rbx         # %rbx存储地址数组的第1个元素的值
  4011b0: 48 8d 44 24 28        lea    0x28(%rsp),%rax         # %rax存储地址数组的第2个元素的地址
  4011b5: 48 8d 74 24 50        lea    0x50(%rsp),%rsi
  4011ba: 48 89 d9              mov    %rbx,%rcx               # %rcx存储地址数组的第1个元素的值
  
  # 下面用i和i+1来表示元素位置
  4011bd: 48 8b 10              mov    (%rax),%rdx             # %rdx存储地址数组的第i+1个元素的值
  4011c0: 48 89 51 08           mov    %rdx,0x8(%rcx)          # 把第i+1和元素的值存储于第i个结构体元素的next域中，next域的地址为0x8(%rcx)的值
  4011c4: 48 83 c0 08           add    $0x8,%rax
  4011c8: 48 39 f0              cmp    %rsi,%rax
  4011cb: 74 05                 je     4011d2 <phase_6+0xde>
  4011cd: 48 89 d1              mov    %rdx,%rcx
  4011d0: eb eb                 jmp    4011bd <phase_6+0xc9>
  ####################################### Section 4 end ######################################
  
  ######################### Section 5：判断结构体数组是否是递减序列 #############################
  4011d2: 48 c7 42 08 00 00 00  movq   $0x0,0x8(%rdx)
  4011d9: 00 
  4011da: bd 05 00 00 00        mov    $0x5,%ebp
  4011df: 48 8b 43 08           mov    0x8(%rbx),%rax
  4011e3: 8b 00                 mov    (%rax),%eax
  4011e5: 39 03                 cmp    %eax,(%rbx)
  4011e7: 7d 05                 jge    4011ee <phase_6+0xfa>
  4011e9: e8 4c 02 00 00        callq  40143a <explode_bomb>
  4011ee: 48 8b 5b 08           mov    0x8(%rbx),%rbx
  4011f2: 83 ed 01              sub    $0x1,%ebp
  4011f5: 75 e8                 jne    4011df <phase_6+0xeb>
  ####################################### Section 5 end ######################################
  
  4011f7: 48 83 c4 50           add    $0x50,%rsp
  4011fb: 5b                    pop    %rbx
  4011fc: 5d                    pop    %rbp
  4011fd: 41 5c                 pop    %r12
  4011ff: 41 5d                 pop    %r13
  401201: 41 5e                 pop    %r14
  401203: c3                    retq
```
