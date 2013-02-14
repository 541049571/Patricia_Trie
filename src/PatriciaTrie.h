#ifndef _PATRICIA_TRIE_H_
#define _PATRICIA_TRIE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_Trie TTrie;

TTrie* Trie_Create(void);
void Trie_Destroy(TTrie *hTrie);
void Trie_Put(TTrie *hTrie, unsigned long key, unsigned long value);
unsigned long Trie_Get(TTrie *hTrie, unsigned long key);
unsigned long Trie_Remove(TTrie *hTrie, unsigned long key);
void Trie_Print(TTrie *hTrie);

#ifdef __cplusplus
}
#endif

#endif /* _PATRICIA_TRIE_H_ */

