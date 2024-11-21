// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  int pincount[32730];
} kmem;


uint64 start_pa = 70 + (8 << 16);


void
kinit()
{
  initlock(&kmem.lock, "kmem");
  memset(kmem.pincount, 0, sizeof(kmem.pincount));
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  int cnt = 0;
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    kmem.pincount[cnt++] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  // printf("kfree: %p", pa);
  // printf("start_pa: %p", start_pa);
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&kmem.lock);
  kmem.pincount[(uint64)pa / PGSIZE - start_pa]--;
  if(kmem.pincount[(uint64)pa / PGSIZE - start_pa] > 0){
    release(&kmem.lock);
    return;
  }else if(kmem.pincount[(uint64)pa / PGSIZE - start_pa] < 0){
    panic("kfree");
  }
  release(&kmem.lock);
  //kmem.pincount[(uint64)pa / PGSIZE - start_pa] == 0
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    kmem.pincount[(uint64)r / PGSIZE - start_pa] = 1;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  return (void*)r;
}

void kpin(void * pa)
{
  acquire(&kmem.lock);
  kmem.pincount[(uint64)pa / PGSIZE - start_pa]++;
  release(&kmem.lock);
}

int isrefone(void *pa){
  if(kmem.pincount[(uint64)pa / PGSIZE - start_pa] == 1){
    return 0;
  }else{
    return -1;
  }
}
