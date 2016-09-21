
boot/boot.elf:     file format elf32-i386


Disassembly of section .entry:

00007c00 <_start>:
    7c00:	31 c0                	xor    %eax,%eax
    7c02:	8e d8                	mov    %eax,%ds
    7c04:	be                   	.byte 0xbe
    7c05:	12                   	.byte 0x12
    7c06:	7c                   	.byte 0x7c

00007c07 <chloop>:
    7c07:	ac                   	lods   %ds:(%esi),%al
    7c08:	84 c0                	test   %al,%al
    7c0a:	74 13                	je     7c1f <bootloader_start>
    7c0c:	b4 0e                	mov    $0xe,%ah
    7c0e:	cd 10                	int    $0x10
    7c10:	eb f5                	jmp    7c07 <chloop>

00007c12 <hellostr_begin>:
    7c12:	73 74                	jae    7c88 <gdtdesc>
    7c14:	61                   	popa   
    7c15:	67 65 30 20          	xor    %ah,%gs:(%bx,%si)
    7c19:	2e                   	cs
    7c1a:	2e                   	cs
    7c1b:	2e                   	cs
    7c1c:	0d                   	.byte 0xd
    7c1d:	0a 00                	or     (%eax),%al

00007c1f <bootloader_start>:
    7c1f:	fa                   	cli    
    7c20:	fc                   	cld    
    7c21:	31 c0                	xor    %eax,%eax
    7c23:	8e d8                	mov    %eax,%ds
    7c25:	8e c0                	mov    %eax,%es
    7c27:	8e d0                	mov    %eax,%ss

00007c29 <seta20.1>:
    7c29:	e4 64                	in     $0x64,%al
    7c2b:	a8 02                	test   $0x2,%al
    7c2d:	75 fa                	jne    7c29 <seta20.1>
    7c2f:	b0 d1                	mov    $0xd1,%al
    7c31:	e6 64                	out    %al,$0x64

00007c33 <seta20.2>:
    7c33:	e4 64                	in     $0x64,%al
    7c35:	a8 02                	test   $0x2,%al
    7c37:	75 fa                	jne    7c33 <seta20.2>
    7c39:	b0 df                	mov    $0xdf,%al
    7c3b:	e6 60                	out    %al,$0x60
    7c3d:	0f 01 16             	lgdtl  (%esi)
    7c40:	88 7c 0f 20          	mov    %bh,0x20(%edi,%ecx,1)
    7c44:	c0 66 83 c8          	shlb   $0xc8,-0x7d(%esi)
    7c48:	01 0f                	add    %ecx,(%edi)
    7c4a:	22 c0                	and    %al,%al
    7c4c:	ea                   	.byte 0xea
    7c4d:	51                   	push   %ecx
    7c4e:	7c 08                	jl     7c58 <start32+0x7>
	...

00007c51 <start32>:
    7c51:	66 b8 10 00          	mov    $0x10,%ax
    7c55:	8e d8                	mov    %eax,%ds
    7c57:	8e c0                	mov    %eax,%es
    7c59:	8e d0                	mov    %eax,%ss
    7c5b:	66 b8 00 00          	mov    $0x0,%ax
    7c5f:	8e e0                	mov    %eax,%fs
    7c61:	8e e8                	mov    %eax,%gs
    7c63:	bc 00 7c 00 00       	mov    $0x7c00,%esp
    7c68:	e8 ad 00 00 00       	call   7d1a <stage0>

00007c6d <spin>:
    7c6d:	eb fe                	jmp    7c6d <spin>
	...

00007c70 <gdt>:
	...
    7c78:	ff                   	(bad)  
    7c79:	ff 00                	incl   (%eax)
    7c7b:	00 00                	add    %al,(%eax)
    7c7d:	9a cf 00 ff ff 00 00 	lcall  $0x0,$0xffff00cf
    7c84:	00                   	.byte 0x0
    7c85:	92                   	xchg   %eax,%edx
    7c86:	cf                   	iret   
	...

00007c88 <gdtdesc>:
    7c88:	17                   	pop    %ss
    7c89:	00 70 7c             	add    %dh,0x7c(%eax)
	...

00007c8e <readdisk>:
    7c8e:	55                   	push   %ebp
    7c8f:	89 e5                	mov    %esp,%ebp
    7c91:	57                   	push   %edi
    7c92:	56                   	push   %esi
    7c93:	53                   	push   %ebx
    7c94:	52                   	push   %edx
    7c95:	8b 75 0c             	mov    0xc(%ebp),%esi
    7c98:	8b 45 10             	mov    0x10(%ebp),%eax
    7c9b:	a9 ff 01 00 00       	test   $0x1ff,%eax
    7ca0:	74 02                	je     7ca4 <readdisk+0x16>
    7ca2:	eb fe                	jmp    7ca2 <readdisk+0x14>
    7ca4:	c1 e8 09             	shr    $0x9,%eax
    7ca7:	89 45 f0             	mov    %eax,-0x10(%ebp)
    7caa:	85 f6                	test   %esi,%esi
    7cac:	74 66                	je     7d14 <readdisk+0x86>
    7cae:	89 f3                	mov    %esi,%ebx
    7cb0:	81 fe 00 02 00 00    	cmp    $0x200,%esi
    7cb6:	76 05                	jbe    7cbd <readdisk+0x2f>
    7cb8:	bb 00 02 00 00       	mov    $0x200,%ebx
    7cbd:	ba f7 01 00 00       	mov    $0x1f7,%edx
    7cc2:	ec                   	in     (%dx),%al
    7cc3:	83 e0 c0             	and    $0xffffffc0,%eax
    7cc6:	3c 40                	cmp    $0x40,%al
    7cc8:	75 f3                	jne    7cbd <readdisk+0x2f>
    7cca:	8b 4d f0             	mov    -0x10(%ebp),%ecx
    7ccd:	81 c9 00 00 00 e0    	or     $0xe0000000,%ecx
    7cd3:	ba f2 01 00 00       	mov    $0x1f2,%edx
    7cd8:	b0 01                	mov    $0x1,%al
    7cda:	ee                   	out    %al,(%dx)
    7cdb:	42                   	inc    %edx
    7cdc:	89 c8                	mov    %ecx,%eax
    7cde:	b9 04 00 00 00       	mov    $0x4,%ecx
    7ce3:	ee                   	out    %al,(%dx)
    7ce4:	c1 e8 08             	shr    $0x8,%eax
    7ce7:	42                   	inc    %edx
    7ce8:	e2 f9                	loop   7ce3 <readdisk+0x55>
    7cea:	b0 20                	mov    $0x20,%al
    7cec:	ee                   	out    %al,(%dx)
    7ced:	ba f7 01 00 00       	mov    $0x1f7,%edx
    7cf2:	ec                   	in     (%dx),%al
    7cf3:	83 e0 c0             	and    $0xffffffc0,%eax
    7cf6:	3c 40                	cmp    $0x40,%al
    7cf8:	75 f3                	jne    7ced <readdisk+0x5f>
    7cfa:	8b 7d 08             	mov    0x8(%ebp),%edi
    7cfd:	b9 80 00 00 00       	mov    $0x80,%ecx
    7d02:	ba f0 01 00 00       	mov    $0x1f0,%edx
    7d07:	fc                   	cld    
    7d08:	f3 6d                	rep insl (%dx),%es:(%edi)
    7d0a:	29 de                	sub    %ebx,%esi
    7d0c:	01 5d 08             	add    %ebx,0x8(%ebp)
    7d0f:	ff 45 f0             	incl   -0x10(%ebp)
    7d12:	eb 96                	jmp    7caa <readdisk+0x1c>
    7d14:	58                   	pop    %eax
    7d15:	5b                   	pop    %ebx
    7d16:	5e                   	pop    %esi
    7d17:	5f                   	pop    %edi
    7d18:	5d                   	pop    %ebp
    7d19:	c3                   	ret    

00007d1a <stage0>:
    7d1a:	55                   	push   %ebp
    7d1b:	89 e5                	mov    %esp,%ebp
    7d1d:	57                   	push   %edi
    7d1e:	56                   	push   %esi
    7d1f:	53                   	push   %ebx
    7d20:	83 ec 1c             	sub    $0x1c,%esp
    7d23:	8b 35 c6 7d 00 00    	mov    0x7dc6,%esi
    7d29:	c1 e6 09             	shl    $0x9,%esi
    7d2c:	56                   	push   %esi
    7d2d:	68 00 10 00 00       	push   $0x1000
    7d32:	68 e0 02 01 00       	push   $0x102e0
    7d37:	e8 52 ff ff ff       	call   7c8e <readdisk>
    7d3c:	83 c4 0c             	add    $0xc,%esp
    7d3f:	81 3d e0 02 01 00 7f 	cmpl   $0x464c457f,0x102e0
    7d46:	45 4c 46 
    7d49:	74 02                	je     7d4d <stage0+0x33>
    7d4b:	eb fe                	jmp    7d4b <stage0+0x31>
    7d4d:	a1 fc 02 01 00       	mov    0x102fc,%eax
    7d52:	8d 98 e0 02 01 00    	lea    0x102e0(%eax),%ebx
    7d58:	0f b7 05 0c 03 01 00 	movzwl 0x1030c,%eax
    7d5f:	c1 e0 05             	shl    $0x5,%eax
    7d62:	01 d8                	add    %ebx,%eax
    7d64:	89 45 e4             	mov    %eax,-0x1c(%ebp)
    7d67:	31 d2                	xor    %edx,%edx
    7d69:	3b 5d e4             	cmp    -0x1c(%ebp),%ebx
    7d6c:	73 38                	jae    7da6 <stage0+0x8c>
    7d6e:	8b 7b 0c             	mov    0xc(%ebx),%edi
    7d71:	81 ff 00 00 01 00    	cmp    $0x10000,%edi
    7d77:	72 28                	jb     7da1 <stage0+0x87>
    7d79:	8b 43 04             	mov    0x4(%ebx),%eax
    7d7c:	01 f0                	add    %esi,%eax
    7d7e:	50                   	push   %eax
    7d7f:	ff 73 10             	pushl  0x10(%ebx)
    7d82:	57                   	push   %edi
    7d83:	e8 06 ff ff ff       	call   7c8e <readdisk>
    7d88:	8b 4b 14             	mov    0x14(%ebx),%ecx
    7d8b:	8b 43 10             	mov    0x10(%ebx),%eax
    7d8e:	83 c4 0c             	add    $0xc,%esp
    7d91:	b2 01                	mov    $0x1,%dl
    7d93:	39 c1                	cmp    %eax,%ecx
    7d95:	76 0a                	jbe    7da1 <stage0+0x87>
    7d97:	29 c1                	sub    %eax,%ecx
    7d99:	01 f8                	add    %edi,%eax
    7d9b:	89 c7                	mov    %eax,%edi
    7d9d:	31 c0                	xor    %eax,%eax
    7d9f:	f3 aa                	rep stos %al,%es:(%edi)
    7da1:	83 c3 20             	add    $0x20,%ebx
    7da4:	eb c3                	jmp    7d69 <stage0+0x4f>
    7da6:	84 d2                	test   %dl,%dl
    7da8:	75 02                	jne    7dac <stage0+0x92>
    7daa:	eb fe                	jmp    7daa <stage0+0x90>
    7dac:	e8 4f 82 00 00       	call   10000 <bootmain>
    7db1:	eb fe                	jmp    7db1 <stage0+0x97>
    7db3:	90                   	nop
    7db4:	90                   	nop
    7db5:	90                   	nop
    7db6:	90                   	nop
    7db7:	90                   	nop
    7db8:	90                   	nop
    7db9:	90                   	nop
    7dba:	90                   	nop
    7dbb:	90                   	nop
    7dbc:	90                   	nop
    7dbd:	90                   	nop
    7dbe:	90                   	nop
    7dbf:	90                   	nop
    7dc0:	90                   	nop
    7dc1:	90                   	nop
    7dc2:	90                   	nop

Disassembly of section .text:

00010000 <bootmain>:
   10000:	55                   	push   %ebp
   10001:	89 e5                	mov    %esp,%ebp
   10003:	83 ec 14             	sub    $0x14,%esp
   10006:	68 b5 01 01 00       	push   $0x101b5
   1000b:	e8 83 01 00 00       	call   10193 <bputs>
   10010:	83 c4 10             	add    $0x10,%esp
   10013:	eb fe                	jmp    10013 <bootmain+0x13>

00010015 <cga_putc>:
   10015:	55                   	push   %ebp
   10016:	89 e5                	mov    %esp,%ebp
   10018:	53                   	push   %ebx
   10019:	a9 00 ff ff ff       	test   $0xffffff00,%eax
   1001e:	75 03                	jne    10023 <cga_putc+0xe>
   10020:	80 cc 07             	or     $0x7,%ah
   10023:	0f b6 d0             	movzbl %al,%edx
   10026:	83 fa 09             	cmp    $0x9,%edx
   10029:	74 5b                	je     10086 <cga_putc+0x71>
   1002b:	7f 07                	jg     10034 <cga_putc+0x1f>
   1002d:	83 fa 08             	cmp    $0x8,%edx
   10030:	74 0e                	je     10040 <cga_putc+0x2b>
   10032:	eb 54                	jmp    10088 <cga_putc+0x73>
   10034:	83 fa 0a             	cmp    $0xa,%edx
   10037:	74 25                	je     1005e <cga_putc+0x49>
   10039:	83 fa 0d             	cmp    $0xd,%edx
   1003c:	74 28                	je     10066 <cga_putc+0x51>
   1003e:	eb 48                	jmp    10088 <cga_putc+0x73>
   10040:	66 8b 15 e0 12 01 00 	mov    0x112e0,%dx
   10047:	66 85 d2             	test   %dx,%dx
   1004a:	74 57                	je     100a3 <cga_putc+0x8e>
   1004c:	4a                   	dec    %edx
   1004d:	66 89 15 e0 12 01 00 	mov    %dx,0x112e0
   10054:	0f b7 d2             	movzwl %dx,%edx
   10057:	30 c0                	xor    %al,%al
   10059:	83 c8 20             	or     $0x20,%eax
   1005c:	eb 3b                	jmp    10099 <cga_putc+0x84>
   1005e:	66 83 05 e0 12 01 00 	addw   $0x50,0x112e0
   10065:	50 
   10066:	b9 50 00 00 00       	mov    $0x50,%ecx
   1006b:	66 a1 e0 12 01 00    	mov    0x112e0,%ax
   10071:	31 d2                	xor    %edx,%edx
   10073:	66 f7 f1             	div    %cx
   10076:	66 a1 e0 12 01 00    	mov    0x112e0,%ax
   1007c:	29 d0                	sub    %edx,%eax
   1007e:	66 a3 e0 12 01 00    	mov    %ax,0x112e0
   10084:	eb 1d                	jmp    100a3 <cga_putc+0x8e>
   10086:	b0 20                	mov    $0x20,%al
   10088:	0f b7 15 e0 12 01 00 	movzwl 0x112e0,%edx
   1008f:	8d 4a 01             	lea    0x1(%edx),%ecx
   10092:	66 89 0d e0 12 01 00 	mov    %cx,0x112e0
   10099:	8b 0d e4 12 01 00    	mov    0x112e4,%ecx
   1009f:	66 89 04 51          	mov    %ax,(%ecx,%edx,2)
   100a3:	66 a1 e0 12 01 00    	mov    0x112e0,%ax
   100a9:	66 3d cf 07          	cmp    $0x7cf,%ax
   100ad:	76 37                	jbe    100e6 <cga_putc+0xd1>
   100af:	8b 0d e4 12 01 00    	mov    0x112e4,%ecx
   100b5:	ba 01 0f 00 00       	mov    $0xf01,%edx
   100ba:	4a                   	dec    %edx
   100bb:	74 0c                	je     100c9 <cga_putc+0xb4>
   100bd:	66 8b 99 a0 00 00 00 	mov    0xa0(%ecx),%bx
   100c4:	66 89 19             	mov    %bx,(%ecx)
   100c7:	eb f1                	jmp    100ba <cga_putc+0xa5>
   100c9:	ba 80 07 00 00       	mov    $0x780,%edx
   100ce:	66 c7 04 51 20 07    	movw   $0x720,(%ecx,%edx,2)
   100d4:	42                   	inc    %edx
   100d5:	81 fa d0 07 00 00    	cmp    $0x7d0,%edx
   100db:	75 f1                	jne    100ce <cga_putc+0xb9>
   100dd:	83 e8 50             	sub    $0x50,%eax
   100e0:	66 a3 e0 12 01 00    	mov    %ax,0x112e0
   100e6:	8b 0d e8 12 01 00    	mov    0x112e8,%ecx
   100ec:	b0 0e                	mov    $0xe,%al
   100ee:	89 ca                	mov    %ecx,%edx
   100f0:	ee                   	out    %al,(%dx)
   100f1:	8d 59 01             	lea    0x1(%ecx),%ebx
   100f4:	66 a1 e0 12 01 00    	mov    0x112e0,%ax
   100fa:	66 c1 e8 08          	shr    $0x8,%ax
   100fe:	89 da                	mov    %ebx,%edx
   10100:	ee                   	out    %al,(%dx)
   10101:	b0 0f                	mov    $0xf,%al
   10103:	89 ca                	mov    %ecx,%edx
   10105:	ee                   	out    %al,(%dx)
   10106:	a0 e0 12 01 00       	mov    0x112e0,%al
   1010b:	89 da                	mov    %ebx,%edx
   1010d:	ee                   	out    %al,(%dx)
   1010e:	5b                   	pop    %ebx
   1010f:	5d                   	pop    %ebp
   10110:	c3                   	ret    

00010111 <bioinit>:
   10111:	55                   	push   %ebp
   10112:	89 e5                	mov    %esp,%ebp
   10114:	56                   	push   %esi
   10115:	53                   	push   %ebx
   10116:	66 a1 00 80 0b 00    	mov    0xb8000,%ax
   1011c:	66 c7 05 00 80 0b 00 	movw   $0xa55a,0xb8000
   10123:	5a a5 
   10125:	66 8b 15 00 80 0b 00 	mov    0xb8000,%dx
   1012c:	66 81 fa 5a a5       	cmp    $0xa55a,%dx
   10131:	74 11                	je     10144 <bioinit+0x33>
   10133:	c7 05 e8 12 01 00 b4 	movl   $0x3b4,0x112e8
   1013a:	03 00 00 
   1013d:	be 00 00 0b 00       	mov    $0xb0000,%esi
   10142:	eb 15                	jmp    10159 <bioinit+0x48>
   10144:	66 a3 00 80 0b 00    	mov    %ax,0xb8000
   1014a:	c7 05 e8 12 01 00 d4 	movl   $0x3d4,0x112e8
   10151:	03 00 00 
   10154:	be 00 80 0b 00       	mov    $0xb8000,%esi
   10159:	b0 0e                	mov    $0xe,%al
   1015b:	8b 15 e8 12 01 00    	mov    0x112e8,%edx
   10161:	ee                   	out    %al,(%dx)
   10162:	89 d0                	mov    %edx,%eax
   10164:	8d 48 01             	lea    0x1(%eax),%ecx
   10167:	89 ca                	mov    %ecx,%edx
   10169:	ec                   	in     (%dx),%al
   1016a:	0f b6 c0             	movzbl %al,%eax
   1016d:	c1 e0 08             	shl    $0x8,%eax
   10170:	89 c3                	mov    %eax,%ebx
   10172:	b0 0f                	mov    $0xf,%al
   10174:	8b 15 e8 12 01 00    	mov    0x112e8,%edx
   1017a:	ee                   	out    %al,(%dx)
   1017b:	89 ca                	mov    %ecx,%edx
   1017d:	ec                   	in     (%dx),%al
   1017e:	89 35 e4 12 01 00    	mov    %esi,0x112e4
   10184:	0f b6 c0             	movzbl %al,%eax
   10187:	09 d8                	or     %ebx,%eax
   10189:	66 a3 e0 12 01 00    	mov    %ax,0x112e0
   1018f:	5b                   	pop    %ebx
   10190:	5e                   	pop    %esi
   10191:	5d                   	pop    %ebp
   10192:	c3                   	ret    

00010193 <bputs>:
   10193:	55                   	push   %ebp
   10194:	89 e5                	mov    %esp,%ebp
   10196:	53                   	push   %ebx
   10197:	8b 5d 08             	mov    0x8(%ebp),%ebx
   1019a:	0f be 03             	movsbl (%ebx),%eax
   1019d:	84 c0                	test   %al,%al
   1019f:	74 08                	je     101a9 <bputs+0x16>
   101a1:	43                   	inc    %ebx
   101a2:	e8 6e fe ff ff       	call   10015 <cga_putc>
   101a7:	eb f1                	jmp    1019a <bputs+0x7>
   101a9:	b8 0a 00 00 00       	mov    $0xa,%eax
   101ae:	5b                   	pop    %ebx
   101af:	5d                   	pop    %ebp
   101b0:	e9 60 fe ff ff       	jmp    10015 <cga_putc>
