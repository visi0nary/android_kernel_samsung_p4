#ifndef __NVHOST_DUMMY_H
#define __NVHOST_DUMMY_H

#include <linux/kobject.h>
#include <linux/platform_device.h>

/*
 * symlink the host1x device in /sys/bus/nvhost/devices/
 */
int nvhost_dummy_symlink(struct platform_device *);

#endif