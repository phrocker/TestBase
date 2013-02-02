
KERNEL_DIR := ../../src/linux-2.6.10
PWD := $(shell pwd)

obj-m := page_alloc.o
#page_alloc-objs :=  allmem.o

default:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) -Wstrict-prototypes modules 

all:	default

clean:	
	rm -f page_alloc.mod.* page_alloc.ko page_alloc.o
	rm -f allmem.mod.* allmem.ko allmem.o

rebuild: clean default
