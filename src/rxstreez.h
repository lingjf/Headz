#ifndef __RXSTREEZ_H_4dbfe8ce_a227_40c5_b4a2_b7bfca15bd51__
#define __RXSTREEZ_H_4dbfe8ce_a227_40c5_b4a2_b7bfca15bd51__

#ifdef __cplusplus
extern "C" {
#endif

/*string radix tree*/

void *rxstree_create(int unique);

void rxstree_destroy(void *handle);

void *rxstree_insert(void *handle, char *idx, void *usr);
void *rxstree_remove(void *handle, char *idx);
void *rxstree_lookup(void *handle, char *idx);

#ifdef __cplusplus
}
#endif

#endif

