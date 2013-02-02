#ifndef ALLMEM_H
#define ALLMEM_H

short verify_memory_range(int zone, unsigned int page_number,unsigned int sector, int page_file_number);

struct page* get_mem_page(int zone,
			unsigned int page_file_number,
			unsigned int page_number,		
			unsigned int sector);

unsigned int page_action(int zone,
			unsigned int page,
			unsigned int sector,
			struct page *pagePointer,
			void *data,
			unsigned long size,
			short action);

struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max);


// high mem function

struct page *get_high_page(struct page *lowPage, 
			unsigned int page_number,
			unsigned int sector);


void free_high_pages(struct page *lowPage);

#endif
