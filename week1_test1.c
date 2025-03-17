#include <stdio.h>
#include <stdlib.h>

#include "weak1_test1_list.h"

#define my_assert(test, message)                                               \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)
#define my_run_test(test)                                                      \
  do {                                                                         \
    char *message = test();                                                    \
    tests_run++;                                                               \
    if (message)                                                               \
      return message;                                                          \
  } while (0)

#define N 1000

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })

static list_item_t items[N];
static list_t l;

static list_t *list_reset(void) {
  for (size_t i = 0; i < N; i++) {
    items[i].value = i;
    items[i].next = NULL;
  }
  l.head = NULL;
  return &l;
}
static int list_size(list_t *l) {
  list_item_t *p = l->head;
  int len = 0;

  while (p) {
    p = p->next;
    len++;
  }

  return len;
}
static void list_insert_before(list_t *l, list_item_t *before,
                               list_item_t *item) {
  list_item_t **p;
  for (p = &l->head; *p != before; p = &(*p)->next)
    ;
  *p = item;
  item->next = before;
}

static list_item_t *merge_two_list(list_item_t *L1, list_item_t *L2) {
  list_item_t *head = NULL;
  list_item_t **indirect = &head;

  while (L1 && L2) {
    if (L1->value <= L2->value) {
      *indirect = L1;
      L1 = L1->next;
    } else {
      *indirect = L2;
      L2 = L2->next;
    }
    indirect = &((*indirect)->next);
  }
  *indirect = (L1) ? L1 : L2;
  return head;
}

static list_item_t *merge_sort_list(list_item_t *head) {
  if (!head || !head->next)
    return head;

  list_item_t *slow = head, *fast = head->next;
  while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
  }
  list_item_t *mid = slow->next;
  slow->next = NULL;

  list_item_t *left = merge_sort_list(head);
  list_item_t *right = merge_sort_list(mid);

  return merge_two_list(left, right);
}

static void merge_sort(list_t *l) {
  if (!l)
    return;
  l->head = merge_sort_list(l->head);
}

static char *test_merge_sort(void) {
  list_reset();
  my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
  for (size_t i = 0; i < N; i++)
    list_insert_before(&l, l.head, &items[i]);

  my_assert(list_size(&l) == N, "List size should be N before sorting");

  merge_sort(&l);

  int expected = 0;
  list_item_t *cur = l.head;
  while (cur) {
    my_assert(cur->value == expected, "merge_sort: unexpected list item value");
    printf("%d ", cur->value);
    expected++;
    cur = cur->next;
  }

  return NULL;
}

static char *test_list(void) {
  /* Test inserting at the beginning */
  list_reset();
  my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
  for (size_t i = 0; i < N; i++)
    list_insert_before(&l, l.head, &items[i]);

  my_assert(list_size(&l) == N, "Final list size should be N");
  size_t k = N - 1;
  list_item_t *cur = l.head;
  while (cur) {
    my_assert(cur->value == k, "Unexpected list item value");
    // printf("%d \n",cur->value);
    k--;
    cur = cur->next;
  }

  /* Test inserting at the end */
  list_reset();
  my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
  for (size_t i = 0; i < N; i++)
    list_insert_before(&l, NULL, &items[i]);
  my_assert(list_size(&l) == N, "Final list size should be N");
  k = 0;
  cur = l.head;
  while (cur) {
    my_assert(cur->value == k, "Unexpected list item value");
    k++;
    cur = cur->next;
  }

  /* Reset the list and insert elements in order (i.e. at the end) */
  list_reset();
  my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
  for (size_t i = 0; i < N; i++)
    list_insert_before(&l, NULL, &items[i]);
  my_assert(list_size(&l) == N, "list size should be N");

  return NULL;
}

int tests_run = 0;

static char *test_suite(void) {
  my_run_test(test_merge_sort);
  return NULL;
}

int main(void) {
  printf("---=[ List tests\n");
  char *result = test_suite();
  if (result)
    printf("ERROR: %s\n", result);
  else
    printf("ALL TESTS PASSED\n");
  printf("Tests run: %d\n", tests_run);
  return !!result;
}
