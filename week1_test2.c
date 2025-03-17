#include <assert.h>
#include <stddef.h>
#include <stdio.h>

typedef struct block {
  size_t size;
  struct block *l, *r;
} block_t;

block_t **find_free_tree(block_t **root, block_t *target) {
  block_t **p = root;
  while (*p && *p != target) {
    if (target->size < (*p)->size)
      p = &((*p)->l);
    else if (target->size > (*p)->size)
      p = &((*p)->r);
    else {
      p = &((*p)->l);
    }
  }
  return p;
}

block_t *find_predecessor_free_tree(block_t **root, block_t *node) {
  if (!node->l)
    return NULL;

  block_t *pred = node->l;
  while (pred->r)
    pred = pred->r;
  return pred;
}

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type
 * block_t) to efficiently locate a block of appropriate size during memory
 * allocation.
 */
void remove_free_tree(block_t **root, block_t *target) {
  /* Locate the pointer to the target node in the tree. */
  block_t **node_ptr = find_free_tree(root, target);

  /* If the target node has two children, we need to find a replacement. */
  if ((*node_ptr)->l && (*node_ptr)->r) {
    /* Find the in-order predecessor:
     * This is the rightmost node in the left subtree.
     */
    block_t **pred_ptr = &(*node_ptr)->l;
    while ((*pred_ptr)->r)
      pred_ptr = &(*pred_ptr)->r;

    /* Verify the found predecessor using a helper function (for debugging).
     */
    block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
    assert(expected_pred == *pred_ptr);

    /* If the predecessor is the immediate left child. */
    if (*pred_ptr == (*node_ptr)->l) {
      block_t *old_right = (*node_ptr)->r;
      *node_ptr = *pred_ptr;      /* Replace target with its left child. */
      (*node_ptr)->r = old_right; /* Attach the original right subtree. */
      assert(*node_ptr != (*node_ptr)->l);
      assert(*node_ptr != (*node_ptr)->r);
    } else {
      /* The predecessor is deeper in the left subtree. */
      block_t *old_left = (*node_ptr)->l;
      block_t *old_right = (*node_ptr)->r;
      block_t *pred_node = *pred_ptr;
      /* Remove the predecessor from its original location. */
      remove_free_tree(&old_left, *pred_ptr);
      /* Replace the target node with the predecessor. */
      *node_ptr = pred_node;
      (*node_ptr)->l = old_left;
      (*node_ptr)->r = old_right;
      assert(*node_ptr != (*node_ptr)->l);
      assert(*node_ptr != (*node_ptr)->r);
    }
  }
  /* If the target node has one child (or none), simply splice it out. */
  else if ((*node_ptr)->l || (*node_ptr)->r) {
    block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
    *node_ptr = child;
  } else {
    /* No children: remove the node. */
    *node_ptr = NULL;
  }

  /* Clear the removed node's child pointers to avoid dangling references. */
  target->l = NULL;
  target->r = NULL;
}

static void inorder_traversal(block_t *node, int *arr, int *idx) {
  if (!node)
    return;
  inorder_traversal(node->l, arr, idx);
  arr[(*idx)++] = (int)node->size;
  inorder_traversal(node->r, arr, idx);
}

/* test for remove_free_tree  */
static void test_remove_free_tree(void) {
  block_t n20, n30, n40, n50, n60, n70, n80;

  n20.size = 20;
  n20.l = n20.r = NULL;
  n40.size = 40;
  n40.l = n40.r = NULL;
  n30.size = 30;
  n30.l = &n20;
  n30.r = &n40;
  n60.size = 60;
  n60.l = n60.r = NULL;
  n80.size = 80;
  n80.l = n80.r = NULL;
  n70.size = 70;
  n70.l = &n60;
  n70.r = &n80;
  n50.size = 50;
  n50.l = &n30;
  n50.r = &n70;

  // set root to 50 
  block_t *root = &n50;

  // call remove_free_tree for 50 root 
  remove_free_tree(&root, &n50);

  // expected to 40 
  assert(root != NULL);
  assert(root->size == 40);

  // inorder_traversal check answer 
  int arr[10] = {0};
  int idx = 0;
  inorder_traversal(root, arr, &idx);

  // expected result：20, 30, 40, 60, 70, 80
  int expected[] = {20, 30, 40, 60, 70, 80};
  int expected_len = 6;

  assert(idx == expected_len);

  for (int i = 0; i < expected_len; i++) {
    assert(arr[i] == expected[i]);
  }

  printf("test_remove_free_tree passed.\n");
}

int main(void) {
  test_remove_free_tree();
  return 0;
}