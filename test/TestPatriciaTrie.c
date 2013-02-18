#include <stdio.h>
#include "../src/PatriciaTrie.h"

typedef struct tag_TestData {
	unsigned long key;
	unsigned long value;
} TTestData;

int TestPatriciaTrie(void)
{
	TTestData testData[] = {
		{ 0x0000,  1 },
		{ 0x0001,  2 },
		{ 0x0002,  3 },
		{ 0x0004,  4 },
		{ 0x0008,  5 },
		{ 0x0010,  6 },
		{ 0x0020,  7 },
		{ 0x0040,  8 },
		{ 0x0080,  9 },
		{ 0x0100, 10 },
		{ 0x0200, 11 },
		{ 0x0400, 12 },
		{ 0x0800, 13 },
		{ 0x1000, 14 },
		{ 0x2000, 15 },
		{ 0x4000, 16 },
		{ 0x8000, 17 },
		{ 0x000F, 18 },
		{ 0xFFFF, 19 },
		{ 0xFF00, 20 },
	};

	TTrie* trie = Trie_Create();

	int size = sizeof(testData) / sizeof(testData[0]);
	for (int i=0; i<size; i++) {
		Trie_Put(trie, testData[i].key, testData[i].value);
	}
	for (int i=0; i<size; i++) {
		const unsigned long outValue = Trie_Get(trie, testData[i].key);
		if (testData[i].value != outValue) {
			printf("Trie_Get() NG! in:%d!=out:%d(key=%08X)\n", testData[i].value, outValue, testData[i].key);
			return 1;
		}
	}
	Trie_Destroy(trie);
	return 0;
}

int main(int argc, const char *argv[])
{
	int ngCount = 0;
	ngCount += TestPatriciaTrie();
	printf("ng count = %d\n", ngCount);
	return 0;
}

