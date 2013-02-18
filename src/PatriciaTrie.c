#include "PatriciaTrie.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define LOG(...) //printf(__VA_ARGS__)

#define BIT_LEN        (32)	/* 4byte */
#define KEY_LEN        (32)	/* 4byte */
#define CHILD_NODE_NUM  (2)	/* bin */

/* 20[byte]/[node] */
typedef struct tag_Node {
	unsigned long  label;
	unsigned long  value;
	unsigned short labellen;
	unsigned short nChild;
	struct tag_Node *childNode[CHILD_NODE_NUM];
} TNode;

struct tag_Trie {
	bool isDead;
	TNode *root;
};


static TNode* CreateChildNode(const unsigned long key, const unsigned long value, const unsigned short len);
static void DestroyNode(TNode *node);
static inline unsigned int CountXorBitMatch(const unsigned long xorBits);
static void ForkNode(TNode *parentNode, TNode *node, const unsigned long key, const unsigned long value, const unsigned short len, const unsigned long diff);
static void PutNode(TNode *parentNode, TNode *node, const unsigned long key, const unsigned long value, const unsigned short len);
static unsigned long GetNode(TNode *node, const unsigned long key, const unsigned short len);
static unsigned long RemoveNode(TNode *node, const unsigned long key);


TTrie* Trie_Create(void)
{
	TTrie *hTrie = (TTrie*)malloc(sizeof(TTrie));
	hTrie->isDead = false;
	hTrie->root = CreateChildNode(0, 0, 0);
	return hTrie;
}

static void DestroyNode(TNode *node)
{
	for (int i=0; i<CHILD_NODE_NUM; i++) {
		if (node->childNode[i]) DestroyNode(node->childNode[i]);
	}
	free(node);
}

void Trie_Destroy(TTrie *hTrie)
{
	assert(hTrie->isDead == false);

	DestroyNode(hTrie->root);
	hTrie->isDead = true;
	free(hTrie);
}

static TNode* CreateChildNode(const unsigned long key, const unsigned long value, const unsigned short len)
{
	TNode *node    = (TNode*)malloc(sizeof(TNode));
	node->label    = key;
	node->value    = value;
	node->labellen = len;
	node->nChild   = 0;
	memset(&node->childNode, 0, sizeof(TNode*)*CHILD_NODE_NUM);
	LOG("	[%s():%d] create child(%08X) key=%08X(len=%d), value=%d\n", __func__, __LINE__, (unsigned long)node, node->label, node->labellen, node->value);
	return node;
}

static inline unsigned int CountXorBitMatch(const unsigned long xorBits)
{
	for (int i=0; i<BIT_LEN; i++) {
		if (xorBits & (0x1<<i)) return i;
	}
	return BIT_LEN;
}

/*
 * [source tree]
 * o-0000
 *  \
 *   0001
 *
 * [append:1101]
 * o-0000
 *  \
 *  (01)-00
 *     \
 *     (11)
 *
 * [append:1111]
 * o-0000
 *  \
 *  (1)----0-00
 *    \     \
 *    (111)  11
 *
 */
static void ForkNode(TNode *parentNode, TNode *node, const unsigned long key, const unsigned long value, const unsigned short len, const unsigned long diff)
{
	const unsigned int nMatch = CountXorBitMatch(diff);

	/* modify node */
	TNode *modifyNode = node;
	modifyNode->label   >>= nMatch;
	modifyNode->labellen -= nMatch;

	/* append node */
	TNode* appendNode = CreateChildNode((key >> nMatch), value, len-nMatch);

	/* insert node */
	const unsigned long filter = 0xffffffff >> (KEY_LEN - nMatch);
	TNode* insertNode  = CreateChildNode((key & filter), 0, nMatch);
	insertNode->childNode[modifyNode->label & 0x1] = modifyNode;
	insertNode->childNode[appendNode->label & 0x1] = appendNode;
	insertNode->nChild = 2;
	parentNode->childNode[insertNode->label & 0x1] = insertNode;

	LOG("	[%s():%d] insert:%08X, modify:%08X, append:%08X\n", __func__, __LINE__,
			(unsigned int)insertNode, (unsigned int)modifyNode, (unsigned int)appendNode);
}

static void PutNode(TNode *parentNode, TNode *node, const unsigned long key, const unsigned long value, const unsigned short len)
{
	LOG("	[%s():%d] key=%08X(len=%d) val=%d\n", __func__, __LINE__, key, len, value);
	if ((len == 1) || (key == node->label)) {
		/* replace */
		node->label = key;
		node->value = value;
		return;
	}

	const unsigned long filter = 0xffffffff >> (KEY_LEN - node->labellen);
	const unsigned long diff   = (key & filter) ^ node->label;
	if (diff) {
		ForkNode(parentNode, node, key, value, len, diff);
		TNode *pnode = parentNode->childNode[key&0x1];
		LOG("	[%s():%d] pnode:%08X, cnode1:%08X, cnode2:%08X\n", __func__, __LINE__,
				(unsigned int)pnode, (unsigned int)pnode->childNode[0], (unsigned int)pnode->childNode[1]);
		return;
	}

	const unsigned long nextKey = key >> node->labellen;
	unsigned int dir = nextKey & 0x1;
	PutNode(node, node->childNode[dir], nextKey, value, (len - node->labellen));
}

void Trie_Put(TTrie *hTrie, unsigned long key, unsigned long value)
{
	LOG("[%s():%d] key=%08X value=%d\n", __func__, __LINE__, key, value);
	assert(hTrie->isDead == false);

	TNode *root = hTrie->root;
	const unsigned int dir = key & 0x1;
	if (root->childNode[dir] == NULL) {
		LOG("	[%s():%d] create root child[dir=%d]\n", __func__, __LINE__, dir);
		root->childNode[dir] = CreateChildNode(key, value, KEY_LEN);
		root->nChild++;
		return;
	}
	PutNode(root, root->childNode[dir], key, value, KEY_LEN);
}

static unsigned long GetNode(TNode *node, const unsigned long key, const unsigned short len)
{
	LOG("	[%s():%d] key=%08X(len=%d) label=%08X(len=%d) value=%d\n", __func__, __LINE__,
			key, len, node->label, node->labellen, node->value);
	if ((len == node->labellen) && (key == node->label)) {
		/* hit */
		LOG("	[%s():%d] hit! key=%08X(len=%d) label=%08X(len=%d) value=%d (node=%08X)\n", __func__, __LINE__,
				key, len, node->label, node->labellen, node->value, (unsigned long)node);
		return node->value;
	}

	const unsigned long nextKey = key >> node->labellen;
	const unsigned int dir = nextKey & 0x1;
	assert(node->childNode[dir] != NULL);

	return GetNode(node->childNode[dir], nextKey, (len - node->labellen));
}

unsigned long Trie_Get(TTrie *hTrie, unsigned long key)
{
	LOG("[%s():%d] key=%08X\n", __func__, __LINE__, key);
	assert(hTrie->isDead == false);

	TNode *root = hTrie->root;
	const unsigned int dir = key & 0x1;
	assert(root->childNode[dir] != NULL);
	return GetNode(root->childNode[dir], key, KEY_LEN);
}

static unsigned long RemoveNode(TNode *node, const unsigned long key)
{
	if (key == node->label) {
		/* T.B.D */
	}
	const unsigned int dir = key & 0x1;
	assert(node->childNode[dir] != NULL);

	return RemoveNode(node->childNode[dir], key);
}

unsigned long Trie_Remove(TTrie *hTrie, unsigned long key)
{
	assert(hTrie->isDead == false);

	TNode *root = hTrie->root;
	const unsigned int dir = key & 0x1;
	assert(root->childNode[dir] != NULL);
	return RemoveNode(root->childNode[dir], key);
}

void Trie_Print(TTrie *hTrie)
{
	/* T.B.D */
}

