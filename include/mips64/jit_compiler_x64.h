/*
* ===========================================================================
* Copyright (C) JIT X86-64 + MIPS-like ASM
* Author: anael-seghezzi
*
* This file is used to dynamically link the x86-64 JIT-compiler for MIPS64.
*
* It was created with the goal of huge speed gains, block caching, dynamic optimization and reduced CPU load.
*
* Link:
* https://gist.github.com/anael-seghezzi/dd89fc64474393b0feec9c0e0de3cb4d
* ===========================================================================
*/

#pragma once

#include <stdint.h>
#include <stdio.h>

#ifdef __TINY_C
#if _WIN32
#include <windows.h>
#include <memoryapi.h>		

#define MEM_SIZE 4096 * 10 // where 4096 - memory where the size will be rounded up to this number, 10 - allocated memory
// MAKING NOW VirtualAlloc() + VirtualFree()
void VirtualAllocateMipsMemory(size_t size_memory, size_t) {
	void* p = NULL;
	p = VirtualAlloc(NULL, MEMSIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (p != NULL) {
		VirtualFree(p, 0, MEM_RELEASE);
	}
	else {
		printf("Error to allocate memory");
		return;
	}
}
/* TODO
	 Maybe on future need to use also VirtualLock() and VirtualUnlock() to manage pages of RAM in physical RAM.
	 After test we'll find out
*/
#else
enum {
	PROT_READ      1,
	PROT_WRITE     2,
	PROT_EXEC      4,
	MAP_PRIVATE    0x02,
	MAP_ANON       0x1000
} Instruction;

void* mmap(void*, size_t, int, int, int, int64_t);
int munmap(void*, size_t);
int mprotect(void*, size_t, int);

#define RTLD_LAZY	0x1
#define RTLD_GLOBAL	0x8
void* dlsym(void* handle, const char* symbol);
void* dlopen(const char* filename, int flag);
int dlclose(void* handle);
#endif

#elif _WIN32
#include <windows.h>
#include <memoryapi.h>	
#else
#include <sys/mman.h>
#include <dlfcn.h>
#ifndef MAP_ANON
#define MAP_ANON 0x1000
#endif
#endif

void x8664_encode_rex(uint8_t* buffer, int dest, int src, int op64) {
	buffer[0] = (dest > 0x7) | ((src > 0x7) << 2) | (op64 << 3) | (0x40);
}

void x8664_encode_modrm(uint8_t* buffer, int dest, int src, int mod) {
	buffer[0] = (dest & 0x7) | ((src & 0x7) << 3) | (mod << 6);
}

void x8664_encode(uint8_t* buffer, int dest, int src, int i, int mod, int op64)
{
	x8664_encode_rex(buffer, dest, src, op64);
	x8664_encode_modrm(buffer + i, dest, src, mod);
}

// registers
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

// This register name mapping was created for Intel Architecture(32/64 bit)

#define XMM0  0
#define XMM1  1
#define XMM2  2
#define XMM3  3
#define XMM4  4
#define XMM5  5
#define XMM6  6
#define XMM7  7
#define XMM8  8
#define XMM9  9
#define XMM10 10
#define XMM11 11
#define XMM12 12
#define XMM13 13
#define XMM14 14
#define XMM15 15

#define AL RAX

#ifdef _WIN32
#define RA1 RCX
#define RA2 RDX
#define RA3 R8
#define RA4 R9
#else
#define RA1 RDI
#define RA2 RSI
#define RA3 RDX
#define RA4 RCX
#define RA5 R8
#define RA6 R9
#endif

#define OPI32(opcode, buffer, im) {\
	buffer[0] = opcode;\
	*(uint32_t*)(buffer + 1) = im;\
	buffer += 5;\
}

#define OP0(opcode, buffer, dest, src) {\
	buffer[0] = 0x0F;\
	buffer[1] = opcode;\
	x8664_encode_modrm(buffer + 2, dest, src, 3);\
	buffer += 3;\
}

#define RR(opcode, buffer, dest, src, op64) {\
	buffer[1] = opcode;\
	x8664_encode(buffer, dest, src, 2, 3, op64);\
	buffer += 3;\
}

#define RR0(opcode, buffer, dest, src, mod, op64) {\
	buffer[1] = 0x0f;\
	buffer[2] = opcode;\
	x8664_encode(buffer, dest, src, 3, mod, op64);\
	buffer += 4;\
}

#define RRF(prefix, opcode, buffer, dest, src, op64) {\
	buffer[0] = prefix;\
	buffer[2] = 0x0f;\
	buffer[3] = opcode;\
	x8664_encode(buffer + 1, src, dest, 3, 3, op64);\
	buffer += 5;\
}

#define RMF(opcode, buffer, dest, src, off) {\
	buffer[0] = 0xf3;\
	buffer[2] = 0x0f;\
	buffer[3] = opcode;\
	*(uint32_t *)(buffer + 5) = off;\
	x8664_encode(buffer + 1, dest, src, 3, 2, 0);\
	buffer += 9;\
}

#define RMF4(opcode, buffer, dest, src, off) {\
	buffer[1] = 0x0f;\
	buffer[2] = opcode;\
	*(uint32_t *)(buffer + 4) = off;\
	x8664_encode(buffer, dest, src, 3, 2, 0);\
	buffer += 8;\
}

#define RI8(opcode, buffer, dest, src, im, mod, op64) {\
	buffer[1] = opcode;\
	buffer[3] = im;\
	x8664_encode(buffer, dest, src, 2, mod, op64);\
	buffer += 4;\
}

#define RI32(opcode, buffer, dest, src, im, mod, op64) {\
	buffer[1] = opcode;\
	*(uint32_t *)(buffer + 3) = im;\
	x8664_encode(buffer, dest, src, 2, mod, op64);\
	buffer += 7;\
}

#define JZ(buffer, off) {\
	*(uint32_t *)(buffer) = 0x840fc085;\
	*(uint32_t *)(buffer + 4) = off;\
	buffer += 8;\
}

#define JNZ(buffer, off) {\
	*(uint32_t *)(buffer) = 0x850fc085;\
	*(uint32_t *)(buffer + 4) = off;\
	buffer += 8;\
}

#define PUSH(buffer, src) {\
	if (src < 8) { buffer[0] = 80 + src; }\
	else {\
		buffer[0] = 0b1000001; buffer++;\
		buffer[0] = 80 + src - 8;\
	}\
	buffer++;\
}

#define POP(buffer, src) {\
	if (src < 8) { buffer[0] = 88 + src; }\
	else {\
		buffer[0] = 0B1000001; buffer++;\
		buffer[0] = 88 + src - 8;\
	}\
	buffer++;\
}

#define CDQ(buffer) { buffer[0] = 0x99; buffer++; }
#define RET(buffer) { buffer[0] = 0xc3; buffer++; }

#define JMP(buffer, off)				 OPI32(0xe9, buffer, off)
#define CALLI32(buffer, off)			 OPI32(0xe8, buffer, off)

#define SETL(buffer, src)				 OP0(0x9c, buffer, 0, src)
#define SETG(buffer, src)				 OP0(0x9f, buffer, 0, src)
#define SETE(buffer, src)				 OP0(0x94, buffer, 0, src)
#define SETA(buffer, src)				 OP0(0x97, buffer, 0, src)
#define SETNE(buffer, src)				 OP0(0x95, buffer, 0, src)
#define MOVZX(buffer, dest, src)	     OP0(0xb6, buffer, dest, src)

#define MOV(buffer, dest, src)			 RR(0x89, buffer, dest, src, 1)
#define ADD(buffer, dest, src)			 RR(0x01, buffer, dest, src, 1)
#define SUB(buffer, dest, src)			 RR(0x29, buffer, dest, src, 1)
#define AND(buffer, dest, src)			 RR(0x21, buffer, dest, src, 1)
#define OR(buffer, dest, src)			 RR(0x09, buffer, dest, src, 1)
#define XOR(buffer, dest, src)			 RR(0x31, buffer, dest, src, 1)
#define IDIV32(buffer, src)				 RR(0xf7, buffer, src, 0b111, 0)
#define IDIV64(buffer, src)				 RR(0xf7, buffer, src, 0b111, 1)
#define SAR1(buffer, src)				 RR(0xd1, buffer, src, 0b111, 1)
#define SAL1(buffer, src)				 RR(0xd1, buffer, src, 0b100, 1)
#define SAR(buffer, src)				 RR(0xd3, buffer, src, 0b111, 1)
#define SAL(buffer, src)				 RR(0xd3, buffer, src, 0b100, 1)
#define CMP32(buffer, dest, src)		 RR(0x39, buffer, dest, src, 0)
#define CMP64(buffer, dest, src)		 RR(0x39, buffer, dest, src, 1)
#define CALL64(buffer, src)				 RR(0xff, buffer, src, 0b010, 1)

#define IMUL(buffer, dest, src)			 RR0(0xaf, buffer, src, dest, 3, 1)
#define UCOMISS(buffer, dest, src)		 RR0(0x2e, buffer, src, dest, 3, 0)
#define MOVUPS(buffer, dest, src)		 RR0(0x10, buffer, src, dest, 3, 0)
#define ADDPS(buffer, dest, src)		 RR0(0x58, buffer, src, dest, 3, 0)
#define SUBPS(buffer, dest, src)		 RR0(0x5c, buffer, src, dest, 3, 0)
#define MULPS(buffer, dest, src)		 RR0(0x59, buffer, src, dest, 3, 0)
#define DIVPS(buffer, dest, src)	     RR0(0x5e, buffer, src, dest, 3, 0)
#define MINPS(buffer, dest, src)		 RR0(0x5d, buffer, src, dest, 3, 0)
#define MAXPS(buffer, dest, src)		 RR0(0x5f, buffer, src, dest, 3, 0)
#define SQRTPS(buffer, dest, src)		 RR0(0x51, buffer, src, dest, 3, 0)

#define ADDI8(buffer, dest, im)			 RI8(0x83, buffer, dest, 0b000, im, 3, 1)
#define SUBI8(buffer, dest, im)			 RI8(0x83, buffer, dest, 0b101, im, 3, 1)
#define SARI(buffer, src, im)			 RI8(0xc1, buffer, src,  0b111, im, 3, 1)
#define SALI(buffer, src, im)			 RI8(0xc1, buffer, src,  0b100, im, 3, 1)
#define CMPI8(buffer, dest, im)			 RI8(0x83, buffer, dest, 0b111, im, 3, 1)

#define MOVI32(buffer, dest, im)         RI32(0xc7, buffer, dest, 0b000, im, 3, 1)
#define ADDI32(buffer, dest, im)         RI32(0x81, buffer, dest, 0b000, im, 3, 1)
#define LOAD64(buffer, dest, src, off)   RI32(0x8b, buffer, src, dest, off, 2, 1)
#define STORE64(buffer, dest, off, src)  RI32(0x89, buffer, dest, src, off, 2, 1)
#define LOAD32(buffer, dest, src, off)   RI32(0x8b, buffer, src, dest, off, 2, 0)
#define STORE32(buffer, dest, off, src)  RI32(0x89, buffer, dest, src, off, 2, 0)

#define MOVSS(buffer, dest, src)		 RRF(0xf3, 0x10, buffer, dest, src, 0)
#define ADDSS(buffer, dest, src)		 RRF(0xf3, 0x58, buffer, dest, src, 0)
#define SUBSS(buffer, dest, src)		 RRF(0xf3, 0x5c, buffer, dest, src, 0)
#define MULSS(buffer, dest, src)		 RRF(0xf3, 0x59, buffer, dest, src, 0)
#define DIVSS(buffer, dest, src)		 RRF(0xf3, 0x5e, buffer, dest, src, 0)
#define MINSS(buffer, dest, src)		 RRF(0xf3, 0x5d, buffer, dest, src, 0)
#define MAXSS(buffer, dest, src)		 RRF(0xf3, 0x5f, buffer, dest, src, 0)
#define SQRTSS(buffer, dest, src)		 RRF(0xf3, 0x51, buffer, dest, src, 0)
#define CVTSI2SS(buffer, dest, src)		 RRF(0xf3, 0x2a, buffer, dest, src, 1)
#define CVTSS2SI(buffer, dest, src)		 RRF(0xf3, 0x2d, buffer, dest, src, 1)
#define PXOR(buffer, dest, src)			 RRF(0x66, 0xef, buffer, dest, src, 0)

#define LOADSS(buffer, dest, src, off)   RMF(0x10, buffer, src, dest, off)
#define STORESS(buffer, dest, off, src)	 RMF(0x11, buffer, dest, src, off)

#define LOADUPS(buffer, dest, src, off)  RMF4(0x10, buffer, src, dest, off)
#define STOREUPS(buffer, dest, off, src) RMF4(0x11, buffer, dest, src, off)

// -----------------------------------------JIT MIPS---------------------------------
struct JITParsing {
	uint8_t* buffer;
	uint8_t** info1;
	uint16_t* info2;
	uint16_t* i1;
};

struct JITState {
	struct JITParsing p;
	uint8_t** info1;
	uint8_t** info2;
	void* buffer;
	int buffer_size;
};

// REGISTERS
#define JIT_R0 -1 // always zero
#ifdef _WIN32
#define JIT_R1 R10 // volatile
#define JIT_R2 R11 // volatile
#else
#define JIT_R1 R8 // volatile
#define JIT_R2 R9 // volatile
#endif
#define JIT_R3 RBX // callee must save
#define JIT_R4 R12 // callee must save
#define JIT_R5 R13 // callee must save
#define JIT_R6 R14 // callee must save
#define JIT_R7 R15 // callee must save

#define JIT_RE RAX // return (very volatile (used internally), use just before calling JIT_RETURN)
#define JIT_SP RSP // stack pointer

// arguments
#define JIT_A1 RA1
#define JIT_A2 RA2
#define JIT_A3 RA3
#define JIT_A4 RA4

// float registers (all volatile)
#define JIT_RF0  XMM0 // arg1, return
#define JIT_RF1  XMM1 // arg2
#define JIT_RF2  XMM2 // arg3
#define JIT_RF3  XMM3 // arg4
#define JIT_RF4  XMM4
#define JIT_RF5  XMM5
#define JIT_RF6  XMM6
#define JIT_RF7  XMM7
#define JIT_RF8  XMM8
#define JIT_RF9  XMM9
#define JIT_RF10 XMM10
#define JIT_RF11 XMM11
#define JIT_RF12 XMM12
#define JIT_RF13 XMM13
#define JIT_RF14 XMM14
#define JIT_RF15 XMM15

#define JIT__INFO(p)\
p->info1[0] = p->buffer;\
p->info1++;\
p->i1++;

#define JIT__INFO2(p)\
p->info2[0] = p->i1;\
p->info2++;

// instructions
void JIT_LOAD32(struct JITParsing* p, int d, int s, int o);
void JIT_STORE32(struct JITParsing* p, int d, int o, int s);
void JIT_LOAD64(struct JITParsing* p, int d, int s, int o);
void JIT_STORE64(struct JITParsing* p, int d, int o, int s);
void JIT_ADD(struct JITParsing* p, int d, int s, int t);
void JIT_ADDI(struct JITParsing* p, int d, int s, int i);
void JIT_SUB(struct JITParsing* p, int d, int s, int t);
void JIT_MUL(struct JITParsing* p, int d, int s, int t);
void JIT_DIV(struct JITParsing* p, int d, int s, int t);
void JIT_SHIFTL(struct JITParsing* p, int d, int s, int t);
void JIT_SHIFTR(struct JITParsing* p, int d, int s, int t);
void JIT_SHIFTLI(struct JITParsing* p, int d, int s, int i);
void JIT_SHIFTRI(struct JITParsing* p, int d, int s, int i);
void JIT_AND(struct JITParsing* p, int d, int s, int t);
void JIT_OR(struct JITParsing* p, int d, int s, int t);
void JIT_XOR(struct JITParsing* p, int d, int s, int t);
void JIT_LESS(struct JITParsing* p, int d, int s, int t);
void JIT_JUMP(struct JITParsing* p, int o);
void JIT_BRANCH(struct JITParsing* p, int s, int t, int o);
void JIT_NBRANCH(struct JITParsing* p, int s, int t, int o);
void JIT_CALL(struct JITParsing* p, int o);
void JIT_CALLEX(struct JITParsing* p, int s);
void JIT_RETURN(struct JITParsing* p);

void JIT_LOADF(struct JITParsing* p, int d, int s, int o);
void JIT_STOREF(struct JITParsing* p, int d, int o, int s);
void JIT_ITOF(struct JITParsing* p, int d, int s);
void JIT_FTOI(struct JITParsing* p, int d, int s);
void JIT_ADDF(struct JITParsing* p, int d, int s, int t);
void JIT_SUBF(struct JITParsing* p, int d, int s, int t);
void JIT_MULF(struct JITParsing* p, int d, int s, int t);
void JIT_DIVF(struct JITParsing* p, int d, int s, int t);
void JIT_LESSF(struct JITParsing* p, int d, int s, int t);
void JIT_MINF(struct JITParsing* p, int d, int s, int t);
void JIT_MAXF(struct JITParsing* p, int d, int s, int t);

void JIT_LOADF4(struct JITParsing* p, int d, int s, int o);
void JIT_STOREF4(struct JITParsing* p, int d, int o, int s);
void JIT_ADDF4(struct JITParsing* p, int d, int s, int t);
void JIT_SUBF4(struct JITParsing* p, int d, int s, int t);
void JIT_MULF4(struct JITParsing* p, int d, int s, int t);
void JIT_DIVF4(struct JITParsing* p, int d, int s, int t);
void JIT_MINF4(struct JITParsing* p, int d, int s, int t);
void JIT_MAXF4(struct JITParsing* p, int d, int s, int t);