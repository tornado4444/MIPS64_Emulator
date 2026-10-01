#pragma once

#include <stdint.h>
#include <stdio.h>

#ifdef __TINY_C
#if _WIN32
	#include <windows.h>
	#include <memoryapi.h>		

	#define MEM_SIZE 4096 * 10 // where 4096 - memory where the size will be rounded up to this number, 10 - allocated memory
	// MAKING NOW VirtualAlloc() + VirtualFree()
	void VirtualAllocateMipsMemory(size_t size_memory, size_t ) {
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

void x64_encode_rex(uint8_t* buffer, int dest, int src, int op64) {
	buffer[0] = (dest > 0x7) | ((src > 0x7) << 2) | (op64 << 3) | (0x40);
}

void x64_encode_modrm(uint8_t* buffer, int dest, int src, int mod) {
	buffer[0] = (dest & 0x7) | ((src & 0x7) << 3) | (mod << 6);
}

void x64_encode(uint8_t* buffer, int dest, int src, int i, int mod, int op64)
{
	x64_encode_rex(buffer, dest, src, op64);
	x64_encode_modrm(buffer + i, dest, src, mod);
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

#define OP32(opcode, buffer, im) {\
	buffer[0] = opcode;\
	*(uint32_t *)(buffer + 1) = im;\
	buffer += 5;\
}