#ifndef __LISTZ_H_0bba448b_2994_4ab7_9544_5d828c33d4de__
#define __LISTZ_H_0bba448b_2994_4ab7_9544_5d828c33d4de__

/*
 *
 *       list -------next-------> node
 *       root <------prev-------- 1st
 *        ^|                       ^|
 *        ||                       ||
 *        ||                       ||
 *    next||prev               prev||next
 *        ||                       ||
 *        ||                       ||
 *        |V                       |v
 *       node --------prev------> node
 *       3th  <-------next------- 2nd
 *
 */

typedef struct line_head
{ /* directional single linked loop list */
   struct line_head *next;
} line_head;

typedef struct list_head
{ /* bidirectional double linked loop list */
   struct list_head *next;
   struct list_head *prev;
} list_head;

#define LINE_HEAD_NULL {(line_head*)0}
#define LIST_HEAD_NULL {(list_head*)0,(list_head*)0}
#define LINE_HEAD_INIT(name) {&(name)}
#define LIST_HEAD_INIT(name) {&(name),&(name)} 

/* void list_null(list_head *node); */
#define list_null(node)  \
   (node)->next = (node)->prev = (list_head*)0   

/* void line_null(line_head/list_head *node) */
#define line_null(node)  \
   (node)->next = (line_head*)0

/* void list_init(list_head *node); */
#define list_init(node)  \
   (node)->next = (node)->prev = (node)

/* void line_init(line_head/list_head *node) */
#define line_init(node)  \
   (node)->next = (node)

/*  int list_empty(list_head *root); */
#define list_empty(root) ((root)->next == (root))

/*  int line_empty(line_head/list_head *root) */
#define line_empty(root) ((root)->next == (root))

#define __list_add(_newl,_prev,_next)  \
   {                                   \
      list_head *newl = (_newl);       \
      list_head *prev = (_prev);       \
      list_head *next = (_next);       \
      next->prev = newl;               \
      newl->next = next;               \
      newl->prev = prev;               \
      prev->next = newl;               \
   }

/* void list_add_next(list_head *node, list_head *base); */
#define list_add_next(node,base)   \
   do {                                         \
      __list_add((node),(base),(base)->next);   \
   } while(0)

/* void list_add_prev(list_head *node, list_head *base); */
#define list_add_prev(node,base)   \
   do {                                         \
      __list_add((node),(base)->prev,(base));   \
   } while(0)

/* void list_add(list_head *node, list_head *base); */
#define list_add(node,base) list_add_next(node,base)

#define list_add_head(node,root) list_add_next(node,root)
#define list_add_tail(node,root) list_add_prev(node,root)

/* void line_add(line_head/list_head *node, line_head/list_head *base); */
#define line_add(node,base)  \
   do {                            \
      (node)->next = (base)->next; \
      (base)->next = (node);       \
   } while(0)

/* void __list_del(list_head *prev, list_head *next); */
#define __list_del(_prev,_next)  \
   {                             \
      list_head *prev = (_prev); \
      list_head *next = (_next); \
      next->prev = prev;         \
      prev->next = next;         \
   }

/* void list_del(list_head *node); */
#define list_del(node) \
   do {                                         \
      __list_del((node)->prev,(node)->next);    \
   } while(0)

/* void list_del_null(list_head *node); */
#define list_del_null(node)  \
   do {                                         \
      __list_del((node)->prev,(node)->next);    \
      list_null(node);                          \
   } while(0)

/* void list_del_init(list_head *node); */
#define list_del_init(node)                     \
   do {                                         \
      __list_del((node)->prev,(node)->next);    \
      list_init(node);                          \
   } while(0)   

/* void line_del_next(line_head/list_head *node); */
#define line_del_next(node)  \
   (node)->next = (node)->next->next 

/* void line_del(line_head/list_head *node); */
#define line_del(node)  \
   do {                             \
      line_head *temp = (node);     \
      while (temp->next != (node))  \
         temp = temp->next;         \
      temp->next = temp->next->next;\
   } while(0)

/* list_head * list_get_head(list_head *root); */
#define list_get_head(root) (list_empty(root)?((list_head*)0):(root)->next)

/* list_head * list_get_tail(list_head *root); */
#define list_get_tail(root) (list_empty(root)?((list_head*)0):(root)->prev)

/* void list_enqueue(list_head *node, list_head *root); */
#define list_enqueue(node,root) list_add_tail(node,root)

/* void list_dequeue(list_head &*node, list_head *root); */
#define list_dequeue(node,root) \
   do {                                  \
      (node) = list_get_head(root);      \
      if (node)                          \
         list_del_init(node);            \
   } while(0) 

/* list_head * list_get_top(list_head *root); */
#define list_get_top(root) (list_empty(root)?((list_head*)0):(root)->next)

/* list_head * list_get_bottom(list_head *root); */
#define list_get_bottom(root) (list_empty(root)?((list_head*)0):(root)->prev)

/* void list_push(list_head *node, list_head *root); */
#define list_push(node,root) list_add_head(node,root)

/* void list_pop(list_head &*node, list_head *root); */
#define list_pop(node,root)  \
   do {                                  \
      (node) = list_get_top(root);       \
      if (node)                          \
         list_del_init(node);            \
   } while(0) 

/* void line_get_top(line_head/list_head *root); */
#define line_get_top(root)  \
   (line_empty(root) ? (line_head*)0 : (root)->next)

/* void line_push(line_head/list_head *node, line_head/list_head *root); */
#define line_push(node,root) line_add(node,root)

/* void line_pop(line_head/list_head &*node, line_head/list_head *root); */
#define line_pop(node,root)  \
   do {                                  \
      (node) = line_get_top(root);       \
      if (node)                          \
         (root)->next=(node)->next;      \
   } while(0) 

/* void list_replace(list_head *_new, list_head *_old); */
#define list_replace(_new,_old)  \
   do {                                \
      (_new)->next = (_old)->next;     \
      (_new)->next->prev = (_new);     \
      (_new)->prev = (_old)->prev;     \
      (_new)->prev->next = (_new);     \
      list_init(_old);                 \
   } while(0)

/* void list_move_head(list_head *node, list_head *root); */
#define list_move_head(node,root)  \
   do {                                         \
      list_head *movl = (node);                 \
      __list_del(movl->prev,movl->next);        \
      list_add_head(movl,root);                 \
   } while(0)

/* void list_move_tail(list_head *node, list_head *root); */
#define list_move_tail(node,root)  \
   do {                                         \
      list_head *movl = (node);                 \
      __list_del(movl->prev,movl->next);        \
      list_add_tail(movl,root);                 \
   } while(0)

/* void list_move(list_head *node, list_head *root); */
#define list_move(node,root) list_move_tail(node,root)

#define __list_splice(_list,_prev,_next)  \
   {                                               \
      list_head *first= (_list)->next;             \
      list_head *last = (_list)->prev;             \
      list_head *prev = (_prev);                   \
      list_head *next = (_next);                   \
      first->prev= prev;                           \
      prev->next = first;                          \
      last->next = next;                           \
      next->prev = last;                           \
   }

/* void list_splice_head(list_head *list, list_head *root); */
#define list_splice_head(list, root)   \
   do {                                         \
      if (!list_empty(list)) {                  \
         __list_splice(list,root,(root)->next); \
         list_init(list);                       \
      }                                         \
   } while(0)

/* void list_splice_tail(list_head *list, list_head *root); */
#define list_splice_tail(list, root)   \
   do {                                         \
      if (!list_empty(list)) {                  \
         __list_splice(list,(root)->prev,root); \
         list_init(list);                       \
      }                                         \
   } while(0)

/* void list_lr(list_head *root); */
#define list_lr(root)   \
   do {	                                    \
      if (!list_empty(root)) {               \
         list_move_tail((root)->next,root);  \
      }                                      \
   } while(0)

/* void list_rr(list_head *root); */
#define list_rr(root)   \
   do {	                                    \
      if (!list_empty(root)) {               \
         list_move_head((root)->prev,root);  \
      }                                      \
   } while(0)

/*below non-prev function is also for line_head*/

#define list_entry(ptr, type, field)  \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->field)))

#define list_for_each(iter, root)  \
	for( (iter) = (root)->next; (iter) != (root); (iter) = (iter)->next )

#define list_for_each_safe(iter, temp, root)  \
	for( (iter) = (root)->next, (temp) = (iter)->next;  \
        (iter) != (root);                              \
        (iter) = (temp), (temp) = (iter)->next )

#define list_for_each_continue(iter, root)  \
	for( (iter) = (iter)->next; (iter) != (root); (iter) = (iter)->next )

#define list_for_each_continue_safe(iter, temp, root)  \
	for( (iter) = (iter)->next, (temp) = (iter)->next;  \
        (iter) != (root);                              \
        (iter) = (temp), (temp) = (iter)->next )

#define list_for_each_from(iter, root)  \
	for( ; (iter) != (root); (iter) = (iter)->next )

#define list_for_each_from_safe(iter, temp, root)  \
	for( (temp) = (iter)->next;                     \
        (iter) != (root);                          \
        (iter) = (temp), (temp) = (iter)->next )

#define list_for_each_prev(iter, root)  \
	for( (iter) = (root)->prev; (iter) != (root); (iter) = (iter)->prev )

#define list_for_each_prev_safe(iter, temp, root)     \
	for( (iter) = (root)->prev, (temp) = (iter)->prev; \
        (iter) != (root);                             \
        (iter) = (temp), (temp) = (iter)->prev )   

#define list_for_each_prev_continue(iter, root)  \
	for( (iter) = (iter)->prev; (iter) != (root); (iter) = (iter)->prev )

#define list_for_each_prev_continue_safe(iter, temp, root)  \
	for( (iter) = (iter)->prev, (temp) = (iter)->prev;       \
        (iter) != (root);                                   \
        (iter) = (temp), (temp) = (iter)->prev )   

#define list_for_each_prev_from(iter, root)  \
	for( ; (iter) != (root); (iter) = (iter)->prev )

#define list_for_each_prev_from_safe(iter, temp, root)  \
	for( (temp) = (iter)->prev;                          \
        (iter) != (root);                               \
        (iter) = (temp), (temp) = (iter)->prev )   

#define list_for_each_entry(iter, root, type, field)     \
	for( (iter) = list_entry((root)->next, type, field);  \
        &(iter)->field != (root);                        \
        (iter) = list_entry((iter)->field.next, type, field) )

#define list_for_each_entry_safe(iter, temp, root, type, field)      \
	for( (iter) = list_entry((root)->next, type, field),              \
           (temp) = list_entry((iter)->field.next, type, field);     \
        &(iter)->field != (root);                                    \
        (iter) = (temp),                                             \
           (temp) = list_entry((temp)->field.next, type, field) )

#define list_for_each_entry_continue(iter, root, type, field)     \
	for( (iter) = list_entry((iter)->field.next, type, field);     \
	     &(iter)->field != (root);	                              \
	     (iter) = list_entry((iter)->field.next, type, field) )

#define list_for_each_entry_continue_safe(iter, temp, root, type, field)   \
	for( (iter) = list_entry((iter)->field.next, type, field),              \
           (temp) = list_entry((iter)->field.next, type, field);           \
        &(iter)->field != (root);                                          \
        (iter) = (temp),                                                   \
           (temp) = list_entry((temp)->field.next, type, field) )

#define list_for_each_entry_from(iter, root, type, field)   \
	for( ;                                                   \
        &(iter)->field != (root);                           \
	     (iter) = list_entry((iter)->field.next, type, field))

#define list_for_each_entry_from_safe(iter, temp, root, type, field)    \
	for( (temp) = list_entry((iter)->field.next, type, field);		      \
	     &(iter)->field != (root);						                     \
	     (iter) = (temp),                                                \
	        (temp) = list_entry((temp)->field.next, type, field) )

#define list_for_each_entry_prev(iter, root, type, field)  \
	for( (iter) = list_entry((root)->prev, type, field);    \
        &(iter)->field != (root);                          \
        (iter) = list_entry((iter)->field.prev, type, field) ) 

#define list_for_each_entry_prev_safe(iter, temp, root, type, field)    \
	for( (iter) = list_entry((root)->prev, type, field),                 \
           (temp) = list_entry((iter)->field.prev, type, field);        \
        &(iter)->field != (root);                                       \
        (iter) = (temp),                                                \
           (temp) = list_entry((temp)->field.prev, type, field) )

#define list_for_each_entry_prev_continue(iter, root, type, field)   \
	for( (iter) = list_entry((iter)->field.prev, type, field);	      \
	     &(iter)->field != (root);	                                 \
	     (iter) = list_entry((iter)->field.prev, type, field) )

#define list_for_each_entry_prev_continue_safe(iter, temp, root, type, field) \
	for( (iter) = list_entry((iter)->field.prev, type, field),                 \
           (temp) = list_entry((iter)->field.prev, type, field);              \
        &(iter)->field != (root);                                             \
        (iter) = (temp),                                                      \
           (temp) = list_entry((temp)->field.prev, type, field) )  

#define list_for_each_entry_prev_from(iter, root, type, field)    \
	for( ;	                                                      \
	     &(iter)->field != (root);	                              \
	     (iter) = list_entry((iter)->field.prev, type, field) )

#define list_for_each_entry_prev_from_safe(iter, temp, root, type, field)     \
	for( (temp) = list_entry((iter)->field.prev, type, field);                 \
        &(iter)->field != (root);                                             \
        (iter) = (temp),                                                      \
           (temp) = list_entry((temp)->field.prev, type, field) ) 

#define __list_for_each_entry(iter, root, field)  list_for_each_entry(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_safe(iter, temp, root, field)  list_for_each_entry_safe(iter, temp, root, typeof(*(iter)), field)
#define __list_for_each_entry_continue(iter, root, field)  list_for_each_entry_continue(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_continue_safe(iter, temp, root, field)  list_for_each_entry_continue_safe(iter, temp, root, typeof(*(iter)), field)
#define __list_for_each_entry_from(iter, root, field)  list_for_each_entry_from(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_from_safe(iter, temp, root, field)  list_for_each_entry_from_safe(iter, temp, root, typeof(*(iter)), field)

#define __list_for_each_entry_prev(iter, root, field)  list_for_each_entry_prev(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_prev_safe(iter, temp, root, field)  list_for_each_entry_prev_safe(iter, temp, root, typeof(*(iter)), field)
#define __list_for_each_entry_prev_continue(iter, root, field)  list_for_each_entry_prev_continue(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_prev_continue_safe(iter, temp, root, field)  list_for_each_entry_prev_continue_safe(iter, temp, root, typeof(*(iter)), field)
#define __list_for_each_entry_prev_from(iter, root, field)  list_for_each_entry_prev_from(iter, root, typeof(*(iter)), field)
#define __list_for_each_entry_prev_from_safe(iter, temp, root, field)  list_for_each_entry_prev_from_safe(iter, temp, root, typeof(*(iter)), field)

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*list_cmp)(void*, list_head*, list_head*);

void list_sort(void *priv, list_head *root, list_cmp cmp);

#ifdef __cplusplus
}
#endif

#endif

