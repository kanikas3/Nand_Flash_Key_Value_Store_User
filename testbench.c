/**
 * Test program using the library to access the storage system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include "kvlib.h"

#define SMALL_TEST_SIZE 128

#define LARGE_TEST_SIZE 2048 * 2

char buffer[LARGE_TEST_SIZE];

static int set_key_to_flash(char* key, char* val)
{
	int ret=0;

	if(key == NULL || val == NULL)
		return -1;

	ret = kvlib_set(key, val);
	if(ret != 0) {
		printf("Error: in writing the key\t%s\n", key);
		return -1;
	}
	return 0;
}

static int get_key_from_flash(char* key, char* val)
{
	int ret=0;

	if(key == NULL || val == NULL)
		return -1;

	buffer[0]='\0';

	ret = kvlib_get(key, buffer);
	if(ret != 0) {
		printf("Error: in reading the key\t%s\n", key);
		return -1;
	}

	if(strcmp(buffer, val) != 0) {
		printf("Error: key %s val got %s expected %s\n", key,
		       buffer, val);
		return -1;
	}
	return 0;
}

static int del_key_from_flash(char* key)
{
	int ret = 0;

	if(key == NULL)
		return 0;

	buffer[0] = '\0';

	ret = kvlib_del(key);
	if(ret != 0) {
		printf("Delete failed for %s \n", key);
		return -1;
	}

	return 0;
}

static double get_latency(struct timeval t0,
			  struct timeval t1, uint64_t num_entries)
{
	double delta = ((double)t1.tv_sec * 1000000 + (double)t1.tv_usec) -
		((double)t0.tv_sec * 1000000 + (double)t0.tv_usec);
	return delta/(1000 * num_entries);
}

static int read_write_test(uint64_t num_entries, int large_keys)
{
	uint64_t i;
	int j;
	int ret;
	struct timeval t0, t1;
	double timeDiff = 0;
	char **key_array = NULL;
	char **val_array = NULL;
	int key_size;
	int val_size;
	int threshold;

	if (large_keys) {
		printf("Performing Large key(3 pages) read-write test for %lu pages \n",
		       num_entries);
		key_size = LARGE_TEST_SIZE;
		val_size = LARGE_TEST_SIZE;
		num_entries /= 4;
	} else {
		printf("Performing Short key(128 bytes) read-write test for %lu pages \n",
		       num_entries);
		key_size = SMALL_TEST_SIZE;
		val_size = SMALL_TEST_SIZE;
	}

	key_array = malloc (sizeof(char *) * num_entries);

	if (!key_array) {
		printf("Allocation for key array failed \n");
		goto fail;
	}

	val_array = malloc (sizeof(char *) * num_entries);

	if (!val_array) {
		printf("Allocation for val array failed \n");
		goto fail;
	}


	for (i = 0; i < num_entries; i++) {
		key_array[i] = malloc ( sizeof(char) * key_size);

		if (!key_array[i]) {
			printf("Allocation for key %lu array failed \n", i);
			goto fail;
		}

		val_array[i] = malloc ( sizeof(char) * val_size);

		if (!val_array[i]) {
			printf("Allocation for val %lu array failed \n", i);
			goto fail;
		}
	}

	ret = kvlib_format();
	if (ret == 0)
		printf("Formatting done successfully\n");
	else {
		printf("Error in formatting the flash\n");
		goto fail;
	}

	if (large_keys) {
		threshold = LARGE_TEST_SIZE / 16;
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "%lu", i);
			j = strlen(key_array[i]);

			for (; j < threshold; j++) {
				if (i % 2 == 0)
					key_array[i][j] = 'A' + ((j +i) % 26);
				else
					key_array[i][j] = 'a' + ((j +i) % 26);
			}

			key_array[i][threshold] = '\0';

			sprintf(val_array[i], "%lu", i);
			j = strlen(val_array[i]);
			for (; j < LARGE_TEST_SIZE - 2 - threshold; j++) {
				val_array[i][j] = 'a' + (j % 26);
			}

			val_array[i][j] = '\0';

			threshold += 512;

			if (threshold > LARGE_TEST_SIZE)
				threshold = LARGE_TEST_SIZE / 16;
		}
	} else {
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "key%lu", i);
			sprintf(val_array[i], "val%lu", i);
		}
	}

	gettimeofday(&t0,NULL);

	for (i = 0; i < num_entries; i++)
		set_key_to_flash(key_array[i], val_array[i]);

	gettimeofday(&t1,NULL);

	timeDiff = get_latency(t0,t1, num_entries);
	printf("Write latency for read-write test:\t %f \tms\n", timeDiff);

	gettimeofday(&t0,NULL);

	for (i=0; i < num_entries; i++) {
		get_key_from_flash(key_array[i], val_array[i]);
	}

	gettimeofday(&t1,NULL);
	timeDiff = get_latency(t0,t1, num_entries);
	printf("Read latency for read-write test:\t %f \tms \n", timeDiff);

	return 0;
fail:
	if (val_array) {
		for (i = 0; i < num_entries; i++) {
			if (val_array[i]) {
				free(val_array[i]);
			} else
				break;
		}
		free (val_array);
	}

	if (key_array) {
		for (i = 0; i < num_entries; i++) {
			if (key_array[i]) {
				free(key_array[i]);
			} else
				break;
		}
		free (key_array);
	}
	return -1;
}

static int delete_test(uint64_t num_entries, int large_keys)
{
	uint64_t i;
	int ret, j;
	struct timeval t0, t1;
	double timeDiff = 0;
	char **key_array = NULL;
	char **val_array = NULL;
	int key_size;
	int val_size;
	int threshold;

	if (large_keys) {
		printf("Performing Large key(3 pages) delete test for %lu pages \n",
		       num_entries);
		key_size = LARGE_TEST_SIZE;
		val_size = LARGE_TEST_SIZE;
		num_entries /= 4;
	} else {
		printf("Performing Short key(128 bytes) delete test for %lu pages \n",
		       num_entries);
		key_size = SMALL_TEST_SIZE;
		val_size = SMALL_TEST_SIZE;
	}

	key_array = malloc (sizeof(char *) * num_entries);

	if (!key_array) {
		printf("Allocation for key array failed \n");
		goto fail;
	}

	val_array = malloc (sizeof(char *) * num_entries);

	if (!val_array) {
		printf("Allocation for val array failed \n");
		goto fail;
	}


	for (i = 0; i < num_entries; i++) {
		key_array[i] = malloc ( sizeof(char) * key_size);

		if (!key_array[i]) {
			printf("Allocation for key %lu array failed \n", i);
			goto fail;
		}

		val_array[i] = malloc ( sizeof(char) * val_size);

		if (!val_array[i]) {
			printf("Allocation for val %lu array failed \n", i);
			goto fail;
		}
	}

	ret = kvlib_format();
	if (ret == 0)
		printf("Formatting done successfully\n");
	else {
		printf("Error in formatting the flash\n");
		goto fail;
	}


	if (large_keys) {
		threshold = LARGE_TEST_SIZE / 16;
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "%lu", i);
			j = strlen(key_array[i]);

			for (; j < threshold; j++) {
				if (i % 2 == 0)
					key_array[i][j] = 'A' + ((j +i) % 26);
				else
					key_array[i][j] = 'a' + ((j +i) % 26);
			}

			key_array[i][threshold] = '\0';

			sprintf(val_array[i], "%lu", i);
			j = strlen(val_array[i]);
			for (; j < LARGE_TEST_SIZE - 2 - threshold; j++) {
				val_array[i][j] = 'a' + (j % 26);
			}

			val_array[i][j] = '\0';

			threshold += 512;

			if (threshold > LARGE_TEST_SIZE)
				threshold = LARGE_TEST_SIZE / 16;
		}
	} else {
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "key%lu", i);
			sprintf(val_array[i], "val%lu", i);
		}
	}

	gettimeofday(&t0,NULL);

	for (i = 0; i < num_entries; i++)
		set_key_to_flash(key_array[i], val_array[i]);

	gettimeofday(&t1,NULL);

	timeDiff = get_latency(t0,t1, num_entries);
	printf("Write latency for delete test:\t %f \tms\n", timeDiff);

	gettimeofday(&t0,NULL);

	for (i = 0; i < num_entries; i++)
		del_key_from_flash(key_array[i]);

	gettimeofday(&t1,NULL);
	timeDiff = get_latency(t0,t1, num_entries);
	printf("Delete latency for delete test:\t %f \tms\n", timeDiff);

	for (i = 0; i < num_entries; i++) {
		ret = kvlib_get(key_array[i], val_array[i]);

		if (!ret) {
			printf("Error: Found key %s on flash ret was %d\n",
			       key_array[i], ret);
		}
	}

	return 0;
fail:
	if (val_array) {
		for (i = 0; i < num_entries; i++) {
			if (val_array[i]) {
				free(val_array[i]);
			} else
				break;
		}
		free (val_array);
	}

	if (key_array) {
		for (i = 0; i < num_entries; i++) {
			if (key_array[i]) {
				free(key_array[i]);
			} else
				break;
		}
		free (key_array);
	}
	return -1;
}

static int update_test(uint64_t num_entries, int large_keys)
{
	uint64_t i;
	int j, ret;
	struct timeval t0, t1;
	double timeDiff = 0;
	char **key_array = NULL;
	char **val_array = NULL;
	int key_size;
	int val_size;
	int threshold;

	if (large_keys) {
		printf("Performing Large key(3 pages) update test for %lu pages \n",
		       num_entries);
		key_size = LARGE_TEST_SIZE;
		val_size = LARGE_TEST_SIZE;
		num_entries /= 4;
	} else {
		printf("Performing Short key(128 bytes) update test for %lu pages \n",
		       num_entries);
		key_size = SMALL_TEST_SIZE;
		val_size = SMALL_TEST_SIZE;
	}


	key_array = malloc (sizeof(char *) * num_entries);

	if (!key_array) {
		printf("Allocation for key array failed \n");
		goto fail;
	}

	val_array = malloc (sizeof(char *) * num_entries);

	if (!val_array) {
		printf("Allocation for val array failed \n");
		goto fail;
	}


	for (i = 0; i < num_entries; i++) {
		key_array[i] = malloc ( sizeof(char) * key_size);

		if (!key_array[i]) {
			printf("Allocation for key %lu array failed \n", i);
			goto fail;
		}

		val_array[i] = malloc ( sizeof(char) * val_size);

		if (!val_array[i]) {
			printf("Allocation for val %lu array failed \n", i);
			goto fail;
		}
	}

	ret = kvlib_format();
	if (ret == 0)
		printf("Formatting done successfully\n");
	else {
		printf("Error in formatting the flash\n");
		goto fail;
	}

	if (large_keys) {
		threshold = LARGE_TEST_SIZE / 16;
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "%lu", i);
			j = strlen(key_array[i]);

			for (; j < threshold; j++) {
				if (i % 2 == 0)
					key_array[i][j] = 'A' + ((j +i) % 26);
				else
					key_array[i][j] = 'a' + ((j +i) % 26);
			}

			key_array[i][threshold] = '\0';

			sprintf(val_array[i], "%lu", i);
			j = strlen(val_array[i]);
			for (; j < LARGE_TEST_SIZE - 2 - threshold; j++) {
				val_array[i][j] = 'a' + (j % 26);
			}

			val_array[i][j] = '\0';

			threshold += 512;

			if (threshold > LARGE_TEST_SIZE)
				threshold = LARGE_TEST_SIZE / 16;
		}
	} else {
		for (i = 0; i < num_entries; i++)
		{
			sprintf(key_array[i], "key%lu", i);
			sprintf(val_array[i], "val%lu", i);
		}
	}

	gettimeofday(&t0,NULL);

	for (i = 0; i <= num_entries/2; i++)
		set_key_to_flash(key_array[i], val_array[i]);

	gettimeofday(&t1,NULL);

	timeDiff = get_latency(t0,t1, num_entries/2);
	printf("Write latency for update test:\t %f \tms\n", timeDiff);

	gettimeofday(&t0,NULL);

	for (i = num_entries - num_entries/2; i < num_entries; i++)
		set_key_to_flash(key_array[i-num_entries + num_entries/2],
			   val_array[i]);

	gettimeofday(&t1,NULL);
	timeDiff = get_latency(t0,t1, num_entries - num_entries/2);
	printf("Update latency for update test:\t %f \tms\n", timeDiff);

	for (i = 0; i < num_entries/2; i++) {
		get_key_from_flash(key_array[i],
			   val_array[i + num_entries - num_entries/2]);
	}

	return 0;
fail:
	if (val_array) {
		for (i = 0; i < num_entries; i++) {
			if (val_array[i]) {
				free(val_array[i]);
			} else
				break;
		}
		free (val_array);
	}

	if (key_array) {
		for (i = 0; i < num_entries; i++) {
			if (key_array[i]) {
				free(key_array[i]);
			} else
				break;
		}
		free (key_array);
	}
	return -1;
}

static void print_help(void) {
	printf("Error: Provide the valid arguments\n");
	printf("./testbench <key - val size> <number of pages> <key_size>\n");
	printf("key_size is 1 for large_pages and 0 for small_pages\n");
	printf("For Choose key - val size options as below\n");
	printf("1 for read/write latency test\n");
	printf("2 for delete keys test\n");
	printf("3 for updating keys/values test\n");
}

int main(int argc, char *argv[])
{
	int choice, large_page, entries;

	if(argc != 4)
	{
		print_help();
		return -1;
	}

	choice = atoi(argv[1]);
	entries = atoi(argv[2]);
	large_page = atoi(argv[3]);

	switch(choice)
	{
		case 1:
			printf("\nRead-Write Latency test\n");
			read_write_test(entries, large_page);
			break;
		case 2:
			printf("\nDelete test\n");
			delete_test(entries, large_page);
			break;
		case 3:
			printf("\nUpdate test\n");
			update_test(entries, large_page);
			break;
		default:
			print_help();
			return -1;
	}
	return 0;
}
