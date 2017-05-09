/**
 * User space library implementation. The library is used by user-space 
 * processes wanting to manipulate the key value-store. It provides high-level,
 * easy to use functions that abstracts the IOCTL interface with the kernel 
 * module through the virtual device
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

/* here we get some info from the virtual device header: device name, major 
 * number, ioctl commands identifiers, and the struct keyval definition */
#include "../kernel/device.h"

/**
 * Called by a process wanting to do a format operation.
 * Returns:
 * 0 on success
 * -1 on error when opening the virtual device file
 * -2 on ioctl error
 * -3 on erase operation error
 */
int kvlib_format()
{
	int fd, ret;

	/* open virtual device file */
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
		return -1; /* error openning device file */

	/* send ioclt command */
	if (ioctl(fd, IOCTL_FORMAT, &ret) != 0)
		ret = -2; /* ioctl error */
	else if (ret != 0)
		ret = -3; /* error during driver erase operation */

	/* close virtual device */
	close(fd);
	return ret;
}

/**
 * Called by a process wanting to write a key/value couple (set)
 * Returns:
 * 0 on success
 * -1 on error when opening the virtual device file
 * -2 on IOCTL error
 * -3 if key/val set operation failed
 */
int kvlib_set(const char *key, const char *value)
{
	int fd;
	int ret = 0;
	keyval kv;

	/* open virtual device file */
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
		return -1;

	/* prepare the keyval structure that will be sent through ioctl */
	kv.key = (char *)malloc((strlen(key) + 1) * sizeof(char));

	if (!kv.key) {
		goto fail;
	}

	kv.val = (char *)malloc((strlen(value) + 1) * sizeof(char));

	if (!kv.val) {
		goto fail;
	}

	sprintf(kv.key, "%s", key);
	sprintf(kv.val, "%s", value);

	kv.key_len = strlen(key);
	kv.val_len = strlen(value);

	/* send ioctl command */
	if (ioctl(fd, IOCTL_SET, &kv) != 0) {
		ret = -2; /* ioctl error */
	} else if (kv.status == -1) {
		ret = -3;
	}
fail:
	/* cleanup */
	if (kv.key)
		free(kv.key);

	if (kv.val)
		free(kv.val);

	/* close virtual device file */
	close(fd);
	return ret;
}


/**
 * Called by a process to delete a key/val pair
 * Returns:
 * 0 when ok
 * -1 on virtual device file open error
 * -2 on IOCTL error
 * -3 if delete did not succeed
 */
int kvlib_del(const char *key)
{
	int fd;
	int ret = 0;
	keyt kv;

	/* open virtual device file */
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
		return -1;

	/* prepare the keyval structure that will be sent through ioctl */
	kv.key = (char *)malloc((strlen(key) + 1) * sizeof(char));

	if (!kv.key) {
		goto fail;
	}

	sprintf(kv.key, "%s", key);

	kv.key_len = strlen(key);

	/* send ioctl command */
	if (ioctl(fd, IOCTL_DEL, &kv) != 0) {
		ret = -2; /* ioctl error */
	} else if (kv.status == -1) {
		ret = -3;
	}
fail:
	/* cleanup */
	if (kv.key)
		free(kv.key);

	/* close virtual device file */
	close(fd);
	return ret;
}

/**
 * Called by a process to get a value from a key
 * Returns:
 * 0 when ok
 * -1 on virtual device file open error
 * -2 on IOCTL error
 * -3 if key not found
 */
int kvlib_get(const char *key, char *value)
{
	int fd;
	int ret = 0;
	keyval kv;

	/* open virtual device file */
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
		return -1;

	/* peprare the keyval structure we will send through IOCTL */
	kv.key = (char *)malloc((strlen(key) + 1) * sizeof(char));

	if (!kv.key) {
		goto fail;
	}

	kv.val = (char *)malloc(8192); //Maximum allowed value
	sprintf(kv.key, "%s", key);
	kv.key_len = strlen(key);

	/* ioctl */
	if (ioctl(fd, IOCTL_GET, &kv) != 0) {
		ret = -2; /* ioctl error */
		goto fail;
	}

	/* get the value... */
	sprintf(value, "%s", kv.val);

	/* ... and the return code */
	if (kv.status == -1)
		ret = -3; /* key not found */
fail:
	if (!kv.key)
		free(kv.key);

	if (!kv.val)
		free(kv.val);

	close(fd);

	return ret;
}
