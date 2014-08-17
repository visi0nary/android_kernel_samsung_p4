/*
 * Moving tegra host to platform bus/driver/device moves the sysfs
 * path to /sys/devices/platform. But there are still things
 * accessing the old path:
 * /sys/bus/nvhost/devices/host1x/syncpt/22/max
 *
 * So this module creates the old path /sys/bus/nvhost/devices
 * and creates a to the actual host1x sysfs path.
 *
 *
 * The commit that caused this problem is:
 * video: tegra: host: use platform bus/driver/device
 */

#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

#include "nvhost_dummy.h"

static struct kset *nvhost_kset;
static struct kset *devices_kset;

/*
 * symlink the host1x device in /sys/bus/nvhost/devices/
 */
int nvhost_dummy_symlink(struct platform_device *dev) {
	int err;

	pr_info("symlinking nvhost_dummy.\n");

	if (dev == NULL) {
		pr_err("symlinking nvhost_dummy: dev is null.\n");
		return -EINVAL;
	}

	if (devices_kset == NULL) {
		pr_err("symlinking nvhost_dummy: devices_kset is null.\n");
		return -EINVAL;
	}


	err = sysfs_create_link(&devices_kset->kobj, &dev->dev.kobj, "host1x");
	pr_info("symlinking nvhost_dummy. err=%d\n", err);

	return err;
}

static int __init nvhost_dummy_init(void)
{
	pr_err("initializing nvhost_dummy.\n");
	/*
	 * Create a kset with the name of "nvhost",
	 * located under /sys/bus/
	 */
	nvhost_kset = kset_create_and_add("nvhost", NULL, &bus_kset->kobj);
	if (!nvhost_kset)
		return -ENOMEM;

	devices_kset = kset_create_and_add("devices", NULL, &nvhost_kset->kobj);
	if (!devices_kset)
		return -ENOMEM;

	return 0;
}

static void __exit nvhost_dummy_exit(void)
{
	kset_unregister(nvhost_kset);
	kset_unregister(devices_kset);
}

/*
 * initialize this module before host1x
 */
fs_initcall(nvhost_dummy_init);
module_exit(nvhost_dummy_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RyanG <decatf@gmail.com>");
