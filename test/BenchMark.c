#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "../src/PatriciaTrie.h"

typedef struct tag_KeyValue {
	unsigned long key;
	unsigned long value;
} TKeyValue;

unsigned long testKeys[] = {
#include "rand.txt"
};

unsigned long SequentialSearch(TKeyValue *data, int size, unsigned long key)
{
	for (int i=0; i<size; i++) {
		if (data[i].key == key) return data[i].value;
	}
	printf("sequential search: key=%08x is not found\n");
	return 0;
}

int main(int argc, const char *argv[])
{
	int dataSize = sizeof(testKeys) / sizeof(testKeys[0]);
	/* prepare data */
	TKeyValue *data = (TKeyValue *)malloc(sizeof(TKeyValue) * dataSize);
	for (int i=0; i<dataSize; i++) {
		data[i].key   = testKeys[i];
		data[i].value = i;
	}
	TTrie* trie = Trie_Create();
	for (int i=0; i<dataSize; i++) {
		Trie_Put(trie, data[i].key, data[i].value);
	}

	printf("BenchMark\n");

	for (int size_i=1000; size_i<=10000; size_i+=500) {
		clock_t startClock1, endClock1;
		clock_t startClock2, endClock2;
		double seqTimeMsec, trieTimeMsec;
		struct timeval startUsecTime1, endUsecTime1;
		struct timeval startUsecTime2, endUsecTime2;
		double seqTimeUsec, trieTimeUsec;

		startClock1 = clock();
		gettimeofday( &startUsecTime1, NULL );
		for (int i=0; i<size_i; i++) {
			if (i != SequentialSearch(data, size_i, data[i].key))
				printf("miss match @SequentialSearch()\n");
		}
		gettimeofday( &endUsecTime1, NULL );
		endClock1 = clock();
		seqTimeMsec = (double)(endClock1 - startClock1)/(CLOCKS_PER_SEC/1000);
		seqTimeUsec = (endUsecTime1.tv_sec - startUsecTime1.tv_sec) + (endUsecTime1.tv_usec - startUsecTime1.tv_usec);

		startClock2 = clock();
		gettimeofday( &startUsecTime2, NULL );
		for (int i=0; i<size_i; i++) {
			if (i != Trie_Get(trie, data[i].key))
				printf("miss match @Trie_Get()\n");
		}
		gettimeofday( &endUsecTime2, NULL );
		endClock2 = clock();
		trieTimeMsec = (double)(endClock2 - startClock2)/(CLOCKS_PER_SEC/1000);
		trieTimeUsec = (endUsecTime2.tv_sec - startUsecTime2.tv_sec) + (endUsecTime2.tv_usec - startUsecTime2.tv_usec);

		printf("Data Size = %d\n", size_i);
		printf("Sequential Search: %.3f[us] %.3f[ms]\n",   seqTimeUsec , seqTimeMsec );
		printf("Patricia Trie:     %.3f[us] %.3f[ms]\n\n", trieTimeUsec, trieTimeMsec);
	}
	free(data);
	return 0;
}

