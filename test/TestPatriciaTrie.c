#include <stdio.h>
#include "../src/PatriciaTrie.h"

#define TEST_ENTER() printf("test %s()	... ", __func__)
#define TEST_PASS()  printf("pass\n")

int TestPutAndGet(void)
{
	TEST_ENTER();
	unsigned long testKeys[] = {
#include "rand.txt"
	};

	TTrie* trie = Trie_Create();

	int size = sizeof(testKeys) / sizeof(testKeys[0]);
	for (int i=0; i<size; i++) {
		Trie_Put(trie, testKeys[i], i);
	}
	for (int i=0; i<size; i++) {
		const unsigned long outValue = Trie_Get(trie, testKeys[i]);
		if (i != outValue) {
			printf("Trie_Get() NG! in:%d!=out:%d(key=%08X)\n", i, outValue, testKeys[i]);
			return 1;
		}
	}
	Trie_Destroy(trie);
	TEST_PASS();
	return 0;
}

int TestReplace(void)
{
	TEST_ENTER();
	unsigned long testKeys[] = {
#include "rand.txt"
	};

	TTrie* trie = Trie_Create();

	int size = sizeof(testKeys) / sizeof(testKeys[0]);
	for (int i=0; i<size; i++) {
		Trie_Put(trie, testKeys[i], i);
	}
	/* replace */
	for (int i=0; i<size; i++) {
		Trie_Put(trie, testKeys[i], i*2);
	}
	for (int i=0; i<size; i++) {
		const unsigned long outValue = Trie_Get(trie, testKeys[i]);
		if (i*2 != outValue) {
			printf("Trie_Get() NG! in:%d!=out:%d(key=%08X)\n", i*2, outValue, testKeys[i]);
			return 1;
		}
	}
	Trie_Destroy(trie);
	TEST_PASS();
	return 0;
}


int main(int argc, const char *argv[])
{
	int ngCount = 0;
	ngCount += TestPutAndGet();
	ngCount += TestReplace();
	printf("ng count = %d\n", ngCount);
	return 0;
}

