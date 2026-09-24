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
