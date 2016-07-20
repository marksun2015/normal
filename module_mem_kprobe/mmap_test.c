#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/module.h>
#include <linux/kprobes.h>

static int vm_enough_memory_ret(struct kretprobe_instance *ri, 
		struct pt_regs *regs)
{
	int retval = regs_return_value(regs);

	if (strstr(current->comm, "test_malloc") && retval == -ENOMEM) {  
		printk("[kprobe]__vm_enough_memory check failed, not enough memory!!\n"); 
	}

	return 0;
}

int vm_enough_memory_entry(struct mm_struct *mm, long pages, 
		int cap_sys_admin)
{
	unsigned long free;

	if (strstr(current->comm, "test_malloc")) {  
		free = global_page_state(NR_FREE_PAGES);
		free += global_page_state(NR_FILE_PAGES);
		free -= global_page_state(NR_SHMEM);

		free += global_page_state(NR_SLAB_RECLAIMABLE);

		printk("[kprobe]vm_enough_memory free pages = %lu, request pages = %ld\n", 
				free, pages);
	}

	jprobe_return();

	return 0;
}

static struct kretprobe mmap_test_kretprobe = {
	.handler		        = vm_enough_memory_ret,
	.entry_handler	= NULL,
	.data_size		= 0,
	.maxactive		= 100,
	.kp = {
		.symbol_name = "__vm_enough_memory",
	},
};

static struct jprobe mmap_test_jprobe = {
	.entry			        = vm_enough_memory_entry,
	.kp = {
		.symbol_name	= "__vm_enough_memory",
	},
};

static int __init mmap_test_init(void)
{
	int ret;

	ret = register_kretprobe(&mmap_test_kretprobe);

	if (ret < 0) {
		printk(KERN_INFO "register_kretprobe failed, returned %d\n",
				ret);

		return -1;
	}	

	ret = register_jprobe(&mmap_test_jprobe);

	if (ret < 0) {
		printk(KERN_INFO "register_jprobe failed, returned %d\n",
				ret);

		return -1;
	}

	return 0;
}

static void __exit mmap_test_exit(void)
{
	unregister_kretprobe(&mmap_test_kretprobe);
	unregister_jprobe(&mmap_test_jprobe);
}

module_init(mmap_test_init)
module_exit(mmap_test_exit)
MODULE_LICENSE("GPL");
