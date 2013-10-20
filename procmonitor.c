#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>

static void procmonitor_check_proc_tree(unsigned long curr);

static struct timer_list procmonitor_timer;

static void procmonitor_check_proc_tree(unsigned long curr)
{
	int ret;
	struct task_struct *task = (struct task_struct*)curr;

	while (task && task->pid != 0) {
        printk(KERN_INFO "parent process: %s, PID: %d", task->comm, task->pid);
        task = task->parent;
	}

	ret = mod_timer(&procmonitor_timer, jiffies + msecs_to_jiffies(800));

	if (ret)
		printk(KERN_INFO "Error when setting timer\n");
}

static int __init procmonitor_init(void)
{
	int ret;

	printk(KERN_INFO "Starting module.\n");

	setup_timer(&procmonitor_timer, procmonitor_check_proc_tree,
		(unsigned long) current);

	ret = mod_timer(&procmonitor_timer, jiffies + msecs_to_jiffies(200));

	if (ret) {
		printk(KERN_INFO "Error when setting timer\n");
		return -1;
	}
	return 0;
}

static void __exit procmonitor_exit(void)
{
	int ret;

	/*
	 * This function only differs from del_timer on SMP: besides deactivating
	 * the timer it also makes sure the handler has finished executing on
	 * other CPUs.
	 */
	ret = del_timer_sync(&procmonitor_timer);

	if (ret)
		printk("Error when removing timer\n");

	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(procmonitor_init);
module_exit(procmonitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kernel BR team");
MODULE_DESCRIPTION("An example of Process Monitor");
