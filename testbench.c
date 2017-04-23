/**
 * Test program using the library to access the storage system
 */

#include <stdio.h>
#include <stdlib.h>
/* Library header */
#include "kvlib.h"

int main(void)
{
	int ret;
	char buffer[128];
	buffer[0] = '\0';

	/* first let's format the partition to make sure we operate on a 
	 * well-known state */
	ret = kvlib_format();
	printf("Formatting done:\n");
	printf(" returns: %d (should be 0)\n", ret);

	/* "set" operation test */
	ret = kvlib_set("key1", "val1");
	printf("Insert 1 (key1, val1):\n");
	printf(" returns: %d (should be 0)\n", ret);

	ret = kvlib_set("key1", "val2");
	printf("Insert 1 (key1, val2):\n");
	printf(" returns: %d (should be 0)\n", ret);

	ret = kvlib_set("key1", "val3");
	printf("Insert 1 (key1, val3):\n");
	printf(" returns: %d (should be 0)\n", ret);

	ret = kvlib_set("key2", "val2");
	printf("Insert 2 (key2, val2):\n");
	printf(" returns: %d (should be 0)\n", ret);


	ret = kvlib_set("key2", "val3");
	printf("Insert 2 (key2, val3):\n");
	printf(" returns: %d (should be 0)\n", ret);

	ret = kvlib_set("key3", "val3");
	printf("Insert 2 (key3, val3):\n");
	printf(" returns: %d (should be 0)\n", ret);

	ret = kvlib_get("key1", buffer);
	printf("Reading the value of key1:\n");
	printf(" returns: %d (should be 0), read: %s (should be val3)\n", ret,
		buffer);

	ret = kvlib_del("key2");
	printf("Deleting the value of key2: %d\n", ret);

	ret = kvlib_get("key2", buffer);
	printf("Reading the value of key2:\n");
	printf(" returns: %d (should be 0), read: %s (should be val3)\n", ret,
		buffer);

	ret = kvlib_get("key35", buffer);
	printf("Reading the value of key35:\n");
	printf(" returns: %d (should be 0), read: %s (should be val35)\n", ret,
		buffer);

	return EXIT_SUCCESS;
}
