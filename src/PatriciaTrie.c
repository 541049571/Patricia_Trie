#include "PatriciaTrie.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define KEY_LEN        (32)	/* 4byte */
#define CHILD_NODE_NUM  (2)	/* bin */

/* 20[byte]/[node] */
typedef struct tag_Node {
	unsigned long  key;
	unsigned long  value;
	unsigned char  keylen;
	unsigned char  isLeaf;
	unsigned short nChild;
	struct tag_Node *childNode[CHILD_NODE_NUM];
} TNode;

struct tag_Trie {
	bool isDead;
	TNode *root;
};

TTrie* Trie_Create(void)
{
	TTrie *hTrie = (TTrie*)malloc(sizeof(TTrie));
	memset(hTrie, 0, sizeof(TTrie));
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

static TNode* CreateChildNode(const unsigned long key, const unsigned long value, const unsigned char len)
{
	TNode *node  = (TNode*)malloc(sizeof(TNode));
	node->key    = key;
	node->value  = value;
	node->keylen = len;
	node->isLeaf = true;
	node->nChild = 0;
	memset(node->childNode, 0, sizeof(TNode)*CHILD_NODE_NUM);
	return node;
}

static void ForkLeaf(TNode *node, const unsigned long key, const unsigned long value, const unsigned char len)
{
	unsigned long keyDiff = (node->key ^ key);
	unsigned int  nMatch  = 0;
	unsigned long nodeKey = 0;

	for (int i=0; i<len; i++) {
		if (keyDiff & 0x1) break;
		nodeKey = (nodeKey<<i) | (key&0x1);
		nMatch++;
	}

	/* move node */
	const unsigned long moveNodeKey = node->key >> nMatch;
	node->childNode[moveNodeKey & 1] = CreateChildNode(moveNodeKey, node->value, node->keylen-nMatch);

	/* append node */
	const unsigned long newNodeKey = key >> nMatch;
	node->childNode[newNodeKey & 1] = CreateChildNode(newNodeKey, value, len-nMatch);

	node->key = nodeKey;
	node->keylen = nMatch;
	node->isLeaf = false;
	node->nChild = 2;
}

static void ForkNode(TNode *node, const unsigned long key, const unsigned long value, const unsigned char len)
{
	/* T.B.D */
}

static unsigned long GenFilter(const unsigned char len)
{
	unsigned long filter = 0;
	for (int i=0; i<len; i++) {
		filter = (filter<<1) | 1;
	}
	return filter;
}

static void PutNode(TNode *node, const unsigned long key, const unsigned long value, const unsigned char len)
{
	if ((len == 1) || (key == node->key)) {
		/* replace */
		node->key   = key;
		node->value = value;
		return;
	}

	if (node->isLeaf) {
		ForkLeaf(node, key, value, len);
		return;
	}

	const unsigned long filter = GenFilter(node->keylen);
	if ((key & filter) ^ (node->key & filter)) {
		/* T.B.D */
		ForkNode(node, key, value, node->keylen);
		return;
	}

	unsigned int dir = key>>node->keylen & 0x1;
	PutNode(node->childNode[dir], key>>node->keylen, value, len-node->keylen);
}

void Trie_Put(TTrie *hTrie, unsigned long key, unsigned long value)
{
	assert(hTrie->isDead == false);

	TNode *root = hTrie->root;
	const unsigned int dir = key & 0x1;
	if (root->childNode[dir] == NULL) {
		/* put */
		root->childNode[dir] = CreateChildNode(key, value, KEY_LEN);
		root->nChild++;
		return;
	}
	PutNode(root->childNode[dir], key, value, KEY_LEN-1);
}

static unsigned long GetNode(TNode *node, const unsigned long key)
{
	if (key == node->key) {
		/* hit */
		return node->value;
	}
	const unsigned int dir = key & 0x1;
	assert(node->childNode[dir] != NULL);

	return GetNode(node->childNode[dir], key);
}

unsigned long Trie_Get(TTrie *hTrie, unsigned long key)
{
	assert(hTrie->isDead == false);

	TNode *root = hTrie->root;
	const unsigned int dir = key & 0x1;
	assert(root->childNode[dir] != NULL);
	return GetNode(root->childNode[dir], key);
}

static void MergeNode()
{
	/* T.B.D */
}

static unsigned long RemoveNode(TNode *node, const unsigned long key)
{
	if (key == node->key) {
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
	return GetNode(root->childNode[dir], key);
}

void Trie_Print(TTrie *hTrie)
{
	/* T.B.D */
}

int main()
{

}

