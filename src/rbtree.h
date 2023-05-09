#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>
#include <stdbool.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

void exchange_color(node_t *, node_t *);
void rbtree_erase_fixup(rbtree *, node_t *, bool);
node_t *rbtree_successor_find(const rbtree *, node_t *);
void rbtree_inOrder(const rbtree *,key_t *, node_t *, int *);
void tree_delete_traverse(rbtree *, node_t *);
void rotate_R(rbtree *, node_t *);
void rotate_L(rbtree *, node_t *);

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);
void rbtree_insert_fixup(rbtree *, node_t *);

#endif  // _RBTREE_H_
