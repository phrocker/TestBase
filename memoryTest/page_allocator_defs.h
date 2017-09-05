#ifndef PAGE_ALLOCATOR_DEFS_H
#define PAGE_ALLOCATOR_DEFS_H



typedef struct
{
	unsigned long long page_number;
	unsigned short page_file_number;
	unsigned int pages_span;
	unsigned long data_size;
	void *data;
} page_writer_structure;


typedef struct
{
	unsigned long long page_number;
	unsigned short page_file_number;
	unsigned int pages_span;
	unsigned long data_size;
	void *data;
} page_reader_structure;


#define MAX_PAGE_BLOCKS 32


typedef struct
{
	int mem_type;
	char block[MAX_PAGE_BLOCKS]; // if set to one, then page will be freed
	
} page_liberator;

#define TRUE 1

// false is the absence of truth
#define FALSE ~TRUE


// sizee of a system pointer
#define POINTER_SIZE sizeof(int)

// indicates the number of pointers to high memory pages that will
// be included within each low memory page during high mem allocation
#define PAGES_PER_PAGE PAGE_SIZE/POINTER_SIZE // hopefully this will be 1024

#define LOW_MEM_ZONE 0
#define HIGH_MEM_ZONE 1

#define MODULE_NAME "page_allocator"

#define FILE_SYSTEM "/proc/"
 
#define LOW_MEM  LOW_MEM_ZONE
#define HIGH_MEM HIGH_MEM_ZONE

#define HIGH_MEM_DIR "high_mem"

#define LOW_MEM_DIR  "low_mem"

#define ALLOCATED_PAGE_DIR "allocated_pages"

#define PAGE_ALLOCATOR_NAME "allocate_pages"

#define MIN_PAGES "pages_min"
#define FREE_PAGES "free_pages"

// actions to perform on pages
#define READ_PAGE 0
#define WRITE_PAGE 1

#endif
