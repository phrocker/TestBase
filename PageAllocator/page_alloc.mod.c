#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x903ac917, "struct_module" },
	{ 0xb498e11f, "proc_mkdir" },
	{ 0x5418796b, "zone_table" },
	{ 0x1a99e971, "remove_proc_entry" },
	{ 0xa295e86a, "create_proc_entry" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x2fd1d81c, "vfree" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x85a32c76, "_spin_unlock_irqrestore" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x9327521d, "_spin_lock_irqsave" },
	{ 0x4292364c, "schedule" },
	{ 0x2e60bace, "memcpy" },
	{ 0xa4deaca9, "__free_pages" },
	{ 0x1b7d4074, "printk" },
	{ 0x93bd23e9, "__alloc_pages" },
	{ 0x40a33c02, "contig_page_data" },
	{ 0xec15a2f2, "_spin_unlock" },
	{ 0xc93cb480, "kunmap" },
	{ 0xe2b31a97, "kmap" },
	{ 0xb9e5ab27, "_spin_lock" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "831A26D7C2C4F29FE7BAD54");
