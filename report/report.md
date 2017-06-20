# Individual Study Report

## Report

In this Individual Study, I plan to implement addtional components for my Operating System Project, namely, I plan to implement Memory Allocation (Malloc, Free), A Fiber Library, And a File System.

## Rewriting Memory Allocation

Memory Allocation is an imoprtant part of the modern operating system, without it, nothing could be done.

Before I started writing this project, the kernel's memory management is very trivial. So in my original implementation, there is a global variable called ```GLOBAL_MEM_START```, indicating where the next free memory region begins. So whenever a new memory is needed, ```GLOBAL_MEM_START``` is incremented to fit the needs. However, this simple scheme has several downsides:

1. memories cannot be reused
2. memories are not page-aligned, so memory protection cannot be done
3. It's hard to keep track of used memory by each process

So instead of having a global variable called ```GLOBAL_MEM_START``` to manage memories, ```mem_map``` is used instead. mem_map is a bitmap that indicates all the free pages inside the memory. 1 indicates it is being used, 0 indicates it is free. Whenver free pages are requested by either the kernle or the user process, ```bitmap_search``` is called to search continugous free pages. Bitmap can also be to free up pages using ```bitmap_nset```. 

With the help of bitmap, fork() can be achieved using COPY On WRITE. 

So this individual study, I implemented ```malloc```, ```free```, and ```kmalloc``` and ```kfree```

In my operating system, each process has its own virtual space dynamically translated into physical address. And the virtual space is divided into several parts, one of which is the heap. Each process has its heap area that can be dynamically extended through ```sbrk() brk()```. sbrk() and brk() both extends the heap segment and allows processes to manage memories through the heap

```
void *do_sbrk( proc_t *caller, size_t size) {

	unsigned long *ptable;
	int nstart,len;
	int *heap_break_bak;
	void *ptr_addr;
	if (size == 0)
		return get_virtual_addr(caller->heap_break,caller);
	
	heap_break_bak = caller->heap_break;

	if (is_addr_in_same_page( heap_break_bak, ( heap_break_bak + size)) ) {
		caller->heap_break = heap_break_bak + size;
		// kprintf("ptr %x ptr+size %x heap_break %x %x\n", heap_break_bak,heap_break_bak + size, caller->heap_break,mem_map[0]);
		return get_virtual_addr(heap_break_bak,caller);
	}
	

	ptable = caller->protection_table;
	len = physical_len_to_page_len(size);
	nstart = bitmap_search(mem_map, MEM_MAP_LEN, len);
	if (nstart != -1) {
		//set mem_map and caller's ptable's corresponding bits to 1
		bitmap_set_nbits(mem_map, MEM_MAP_LEN, nstart, len);
		bitmap_set_nbits(ptable, PROTECTION_TABLE_LEN, nstart, len);
		ptr_addr = (void *)(nstart * 1024);

		if (is_addr_in_consecutive_page( heap_break_bak, ptr_addr)){
			
			caller->heap_break = heap_break_bak + size ;
			// kprintf("ptr %x pt %x obrk %x brk %x\n", heap_break_bak, mem_map[0], heap_break_bak,caller->heap_break);
			return get_virtual_addr(heap_break_bak,caller);
		}
		if((int *)ptr_addr > heap_break_bak)
			caller->heap_break = (int *)ptr_addr + size;

		// kprintf("sbrk: optr %x ", ptr_addr);
		ptr_addr = get_virtual_addr(ptr_addr,caller);
		// kprintf("ptr %x pt %x obrk %x brk %x\n", ptr_addr, mem_map[0],  heap_break_bak, caller->heap_break);
		return ptr_addr;
	}else{
		kprintf("System out of memory\n");
		for (nstart = 0; nstart < 32; ++nstart)
		{
			kprintf(" %x |",mem_map[nstart]);
		}
		kprintf("\n");
	}
	return NULL;
}
```

In my OS, virtual memory is very simple, Virtual Address = Physical Address + rbase, where rbase is where the first line of the code of this process resides in the physical memory. when sbrk is called, the kernel checks if the current page can be extended, if the answer is yes, then the end of the heap will be extended to the new limit. On the other hand, if the new limit will be on the next page, and next page is free, then sbrk() will be successful, and heap will be extended to the next page.
However, if the next page isn't free, 

In the user space, heap memory are managed by ```malloc free```. In each of the memory segments being allocated, a header is present that has the meta information about this memory. 

```
typedef struct s_block {
	size_t size;
	struct s_block *next;
	struct s_block *prev;
	int free;
	void *ptr; //a pointer to the allocated block
	char data[1]; //the pointer where the real data is pointed at. 					//b->data is what malloc returns
					//ptr equal the address of data
}block_t;
```

Internally both of malloc and free maintains an internal doubly linkedlist of all the memories being allocated. Upon calling malloc, first fit algorithms is used. Tt traverses through the double linked list, using prev and next pointer, to find one part of the memory that is free and is large enough to hold the request. If no memory holes that can be found, sbrk() is called to extend the heap. Upon returning the new address, the header struct is inserted before the memory hole.

when free is called, it looks at the header file, check if ptr points to the requested block. If yes, then this memory block is marked as free.

However, when requests get larger and larger, the heap segment may become scattered around, there could be the case where there are several consecutive small free memory holes. Memory fragmentation slows the search time. These holes can be potentially merged into a single one to improve performance. 

For ```kmalloc free```, they are pretty much the same with malloc except one tiny difference. When no free and large enough memory blocks can be found, it calls ```get_free_page``` to request new pages. If the request size is smaller than the size of a page, then the rest of the page will be initialised with a new free memory segment.


### Fiber Library

In my fiber library, ```ucontext.h``` is used to implement the fiber.
ucontext is one of the C library for context control, It allows more advanced control flow patterns in C like iterator, fiber, and coroutines. 

Because I am working on a special Architecture called WRAMP, which is a MIPS like architecture. Since no ucontext library has been implemented yet, So I wrote the ucontext in this project.

```
typedef struct _ucontext_t{
	
	/* Process State */
	unsigned long regs[REGS_NR];	//Register values
	unsigned long *sp;
	void *ra;
	void (*pc)();

	// stack_t uc_stack;
	unsigned long  *ss_sp;     /* address of stack */
	int    ss_flags;  /* Flags */
	size_t ss_size;   /* Number of bytes in stack */

	struct _ucontext_t *uc_link;
}ucontext_t;
```

The process state is a machine dependent part that includes all the registers values, return address register, stack pointer. stack_t is the alternative stack that may be used for this ucontext. uc_link links to the next ucontext_t for context switching.

There are four library functions in ucontext 
```
int  getcontext(ucontext_t *ucp);
int  setcontext(const ucontext_t *ucp);
void makecontext(ucontext_t *, void (* func)(), int argc, ...);
int  swapcontext(ucontext_t *oucp, const ucontext_t *ucp);
```

Getcontext saves the current context into the ucp. 

Setcontext transfers the control to the context in ucp. 

Make context sets up an alternative thread of control in ucp. Stack should be set up appropriately so that it does not interfere with the current context's stack. Execution will begin at the entry point of ```func```, and arguments are passed onto the stack. 

swapcontext() saves the current context into oucp, and transfers the control to ucp.

```
.global _ctx_start
_ctx_start:
	lw $1, 0($sp)
	addui $sp, $sp, 1
	jalr $1
	sw $13, 0($sp)
	j _ctx_end


void _ctx_end(ucontext_t *ucp){

	if(ucp->uc_link == NULL)
		sys_exit(0);
	setcontext(ucp->uc_link);

	//should never get here
	sys_exit(2);
}

void makecontext(ucontext_t *ucp, void (* func)(), int argc, ...){
	int *args = &argc + 1;
	uint32_t **spp;
	uint32_t *sp;
	if(ucp == NULL)
		return;

	ucp->pc = (void (*)())&_ctx_start;

	//allocate stack for the ucp context
	spp = (!ucp->ss_flags) ? &ucp->ss_sp : &ucp->sp;
	*spp -= (argc + 2);
	sp = *spp;

	/**
	 * Arrange the stack as follows:
	 * 	func
	 * 	arg1
	 *  ...
	 *  argn
	 *  ucp
	 *
	 * 	The PC of the ucp will set to _ctx_start (refer to lib/ucontext.s)
	 * 	_ctx_start will pop the top of the stack, which is func. After that, the Stack 
	 * 	will be arranged such that all args are left on the top of the  stack. 
	 * 	Then func() is called.
	 * 	When func() returns, _ctx_start will pop ucp, then call 	*	_ctx_end()and pass 
	 * 	the parameter to _ctx_end
	**/
	*sp++ = (int)func;
	memcpy(sp,args,argc);
	sp += argc;
	*sp = (uint32_t)ucp;

}
```

Make context sets up the alternative stack, and point the next control flow to _ctx_start(), _ctx_start will then call func(), and pass the parameters. On successful completion, _ctx_end() is called. _ctx_end checks if the next ucontext is present, if yes, it transfers the control to the next ucontext. otherwise exit() is called.

With the help of ucontext, then fiber library can be implemented.

```
int spawnFiber( void (*func)(void) )
{
	if ( numFibers == MAX_FIBERS ) return LF_MAXFIBERS;
	
	/* Add the new function to the end of the fiber list */
	getcontext( &fiberList[numFibers].context );

	/* Set the context to a newly allocated stack */
	fiberList[numFibers].context.uc_link = 0;
	fiberList[numFibers].stack = malloc( FIBER_STACK );
	fiberList[numFibers].context.uc_stack.ss_sp = fiberList[numFibers].stack;
	fiberList[numFibers].context.uc_stack.ss_size = FIBER_STACK;
	fiberList[numFibers].context.uc_stack.ss_flags = 0;
	
	if ( fiberList[numFibers].stack == 0 )
	{
		LF_DEBUG_OUT( "Error: Could not allocate stack." );
		return LF_MALLOCERROR;
	}
	
	/* Create the context. The context calls fiberStart( func ). */
	makecontext( &fiberList[ numFibers ].context, (void (*)(void)) &fiberStart, 1, func );
	++ numFibers;
	
	return LF_NOERROR;
}
```


