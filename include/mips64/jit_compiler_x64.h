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
#endif
#endif