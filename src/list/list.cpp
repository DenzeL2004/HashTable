#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "list.h"

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"


static int              InitListData        (List *list);

static int              ListResize          (List *list);

static int              ListRecalloc        (List *list, int ver_resize);

static void             InitNode            (Node *list_elem, elem_t val, size_t next, size_t prev);

inline static size_t    CheckCorrectIndex   (const List *list, const size_t ind);

static int              NotFreeNodeValidity (const List *list);

static int              FreeNodeValidity    (const List *list);

static uint64_t         ListCheck           (const List *list);


static int      ListDrawLogicalGraph    (const List *list);

static void     DrawListVariable        (const List *list, FILE *fpout);

static void     PrintListErrors         (uint64_t err, FILE *fpout);

#define REPORT(...)                                         \
    {                                                       \
        ListDump (list, __VA_ARGS__);                       \
        ERR_REPORT (__VA_ARGS__);                           \
                                                            \
    }while (0)

//======================================================================================

int ListCtor (List *list, const size_t capacity)
{
    assert (list != nullptr && "list is nullptr");

    list->tail_ptr = Invalid_ind;
    list->head_ptr = Invalid_ind;
    list->free_ptr = 1;

    list->is_linearized = 1;

    list->size_data      = 0;
    list->capacity       = capacity;
    list->cnt_free_nodes = 0;

    list->data = (Node*) calloc (capacity + 1, sizeof (Node));

    if (CheckNullptr(list->data))
        return PROCESS_ERROR(LIST_CTOR_ERR, "Memory allocation error\n");
    
    InitNode (&list->data[0], Poison_val, Invalid_ind, Invalid_ind);

    if (InitListData(list))
        return PROCESS_ERROR (LIST_CTOR_ERR, "Node initialization error\n");

    return 0;
}

//======================================================================================

int ListDtor (List *list)
{
    assert (list != nullptr && "list ptr is nullptr");


    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListDtor\n");
        return LIST_DTOR_ERR;
    }
    
    #endif

    if (CheckNullptr (list->data))
        LOG_REPORT ("Data is nullptr in dtor\n");
    else    
        free (list->data);

    list->tail_ptr = Poison_ptr;
    list->head_ptr = Poison_ptr;
    list->free_ptr = Poison_ptr;

    list->size_data         = -1;
    list->capacity          = -1;
    list->cnt_free_nodes    = -1;
    
    list->is_linearized = -1;

    return 0;
}

//======================================================================================

static int InitListData (List *list)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: InitListData\n");
        return DATA_INIT_ERR;
    }

    #endif

    size_t Last_used_node = MAX (list->tail_ptr, list->head_ptr);

    for (size_t ip = Last_used_node + 1; ip < list->capacity; ip++) 
        InitNode (list->data + ip, Poison_val, ip + 1, Identifier_free_node);

    InitNode (list->data + list->capacity, Poison_val, Identifier_free_node, Identifier_free_node);


    list->cnt_free_nodes = list->capacity - list->size_data;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: InitListData\n");
        return DATA_INIT_ERR;
    }

    #endif

    return 0;
}

//======================================================================================

static void InitNode (Node *list_elem, elem_t val, size_t next, size_t prev)
{
    assert (list_elem != nullptr && "list_elem ptr is nullptr");

    list_elem->val  = val;
    list_elem->next = next;
    list_elem->prev = prev; 

    return;
}

//======================================================================================

size_t ListInsertPrevInd (List *list, const size_t ind, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertPrevInd,"
                        " ind = %d\n", ind);
        return LIST_INSERT_ERR;
    }

    #endif
    
    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");

    if (!CheckCorrectIndex (list, ind) && ind != Invalid_ind)
        return PROCESS_ERROR (LIST_INSERT_ERR, "Incorrect ind = %d\n", ind);
      

    
    if (list->free_ptr == Invalid_ind)
        return PROCESS_ERROR (LIST_INSERT_ERR, "No free space in list\n");


    if (list->data[ind].prev == Identifier_free_node)
    {
        return PROCESS_ERROR (LIST_INSERT_ERR, "There is nothing at this pointer: %d.\n" 
                                        "You can only add an element before initialized elements\n", ind);
    }

        
    if (list->head_ptr != ind && list->tail_ptr != ind)
        list->is_linearized = 0;

    
    size_t  prev_ptr      = ind;
    size_t  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    size_t next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Invalid_ind].next;
    list->tail_ptr = list->data[Invalid_ind].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertPrevInd,"
                        " ind = %lu\n", ind);
        return LIST_INSERT_ERR;
    }

    #endif

    return cur_free_ptr;
}

//======================================================================================

size_t ListInsertFront (List *list, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertFront %d\n", val);
        return LIST_INSERT_ERR;
    }

    #endif
    
    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
        
    if (list->size_data != 0)
        list->is_linearized = 0;

    
    size_t  prev_ptr      = Invalid_ind;
    size_t  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    size_t next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Invalid_ind].next;
    list->tail_ptr = list->data[Invalid_ind].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertFront %d\n", val);
        return LIST_INSERT_ERR;
    }

    #endif

    return cur_free_ptr;
}

//======================================================================================

size_t ListInsertBack (List *list, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertBack %d\n", val);
        return LIST_INSERT_ERR;
    }

    #endif

    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
    
    
    size_t  prev_ptr      = list->tail_ptr;
    size_t  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    size_t next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Invalid_ind].next;
    list->tail_ptr = list->data[Invalid_ind].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertBack %d\n", val);
        return LIST_INSERT_ERR;
    }

    #endif

    return cur_free_ptr;
}

//======================================================================================

int ListErase (List *list, const size_t ind) 
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListErase, ind = %lu\n", ind);
        return LIST_ERASE_ERR;
    }
   
    #endif
    
    if (!CheckCorrectIndex (list, ind))
        return PROCESS_ERROR (LIST_ERASE_ERR, "Incorrect ind = %lu\n", ind);


    if (list->data[ind].prev == Identifier_free_node)
    {
        LOG_REPORT ("There is nothing at this index: %lu.\n" 
                    "You cannot free a previously freed node\n", ind);
        return LIST_ERASE_ERR;
    }


    if (list->head_ptr != ind && list->tail_ptr != ind)
        list->is_linearized = 0;
    

    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
    

    size_t  cur_ptr   = ind;
    size_t  prev_ptr  = list->data[ind].prev;
    size_t  next_ptr  = list->data[ind].next;

    list->data[prev_ptr].next = next_ptr;
    list->data[next_ptr].prev = prev_ptr;

    InitNode (list->data + cur_ptr, 
              Poison_val, list->free_ptr, Identifier_free_node);

    list->free_ptr = cur_ptr;

    list->head_ptr = list->data[Invalid_ind].next;
    list->tail_ptr = list->data[Invalid_ind].prev;

    list->size_data--;
    list->cnt_free_nodes++;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListErase exit, ind = %lu\n", ind);
        return LIST_ERASE_ERR;
    }  

    #endif

    return 0;
}

//======================================================================================

static int ListResize (List *list)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListResize\n");
        return PTR_OUT_OF_MEMORY_ERR;
    }   

    #endif

    if (list->capacity / 4  <= list->size_data   && 
        list->size_data + 1 < list->capacity / 2 && 
        list->is_linearized == 1)
    {
        list->capacity = list->capacity / 2 + 1;
        return 1;
    } 

    if (list->capacity == list->size_data + 1)
    {
        list->capacity = list->capacity * 2 + 1;
        return 1;
    }

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListResize\n");
        return PTR_OUT_OF_MEMORY_ERR;
    }  

    #endif

    return 0;
}

//======================================================================================

static int ListRecalloc (List *list, int resize_status)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListRecalloc\n");
        return LIST_RECALLOC_ERR;
    } 

    #endif

    if (resize_status == 0) return 0;

    if (resize_status < 0)
    {
        return PROCESS_ERROR (LIST_RECALLOC_ERR, "The list is not subject to recalloc\n");
    }

    list->data = (Node*) realloc (list->data, (list->capacity + 1) * sizeof (Node));

    if (CheckNullptr (list->data))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "List data is nullptr after use recalloc\n");

    
    list->cnt_free_nodes = 0;
    if (InitListData (list))
        return PROCESS_ERROR (LIST_RECALLOC_ERR, "List data initialization error\n");
     
    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListRecalloc\n");
        return LIST_RECALLOC_ERR;
    } 

    #endif

    return 0;
}

//======================================================================================

int ListLinearize (List *list)
{
    assert (list != nullptr && "list is nullptr\n");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListLinearize\n");
        return LIST_LINEARIZE_ERR;
    } 

    #endif

    if (list->is_linearized == 1) 
        return 0;

    Node* new_data = (Node*) calloc (list->capacity + 1, sizeof (Node));

    if (CheckNullptr (new_data))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Linearize error, new data memory allocation error\n");

    size_t logical_ind = list->head_ptr;

    InitNode (new_data, Poison_val, 1l, list->size_data);

    size_t counter = 1;

    while (counter <= list->size_data - 1)
    {
        InitNode (new_data + counter, 
                   list->data[logical_ind].val, counter + 1, counter - 1);
        counter++;

        logical_ind = list->data[logical_ind].next;

        if (logical_ind != 0 && !CheckCorrectIndex (list, logical_ind))
        {
            return PROCESS_ERROR (LIST_LINEARIZE_ERR, "Incorrect list traversal,"
                                                     "logical_ind = %d\n", logical_ind);
        }
    }

    InitNode (new_data + counter, 
               list->data[logical_ind].val, Invalid_ind, counter - 1);


    free (list->data);

    list->cnt_free_nodes = 0;

    list->data = new_data;
    list->head_ptr = list->data[Invalid_ind].next;
    list->tail_ptr = list->data[Invalid_ind].prev;

    if (InitListData (list))
        return PROCESS_ERROR (LIST_LINEARIZE_ERR, "List data initialization error\n");

    list->is_linearized = 1;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListLinearize\n");
        return LIST_LINEARIZE_ERR;
    }

    #endif

    return 0;;
}

//======================================================================================

size_t GetLogicalIndex (const List *list, const size_t ind)
{
    assert (list != nullptr && "list is nullptr\n");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: GetLogicalIndex, ind = %lu\n", ind);
        return GET_LOGICAL_PTR_ERR;
    }   

    #endif

    if (ind > list->size_data || ind == Invalid_ind)
        return PROCESS_ERROR (GET_LOGICAL_PTR_ERR, "Number of elements of the requested index.\nind = %lu\n", ind);

    if (list->is_linearized)
    {
        return list->head_ptr + ind - 1;
    }

    else
    {   
        size_t logical_ind = list->head_ptr;
        size_t counter = 1;

        while (counter < ind)
        {
           logical_ind = list->data[logical_ind].next;
            counter++;            
        }
        
        return logical_ind;
    }

    //No list re-validation as list items don't change
    
} 

//======================================================================================

elem_t ListGetVal (const List *list, const size_t ind)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListGetVal, ind = %d\n", ind);
        return Poison_val; 
    }

    #endif

    if (ind > list->size_data || ind == Invalid_ind)
    {
        PROCESS_ERROR (GET_VAL_ERR, "Incorrect ind = %lu\n", ind);
        return Poison_val;
    }

    //No list re-validation as list items don't change

    return list->data[ind].val;
}

//======================================================================================

size_t ListFindVal (const List *list, const elem_t val)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        PROCESS_ERROR (GET_VAL_ERR, "Incorrect ind = %lu\n", ind);
        return nullptr; 
    }

    #endif

    size_t size = list->size_data;

    size_t cur_ind = list->head_ptr;
    
    elem_t cur_str = Dummy_element;

    while (size--)
    {
        cur_str = list->data[cur_ind].val;

        if (!StrcmpIntrinsic(cur_str, val))                           
        {
            return cur_ind;
        }


        cur_ind = list->data[cur_ind].next;
    }

    //No list re-validation as list items don't change

    return Invalid_ind;
}

//======================================================================================

int ListChangeVal (const List *list, const size_t ind, const elem_t val)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListChangeVal,"
                       " ind = %lu\n", ind);
        return LIST_GET_VAL_ERR; 
    } 

    #endif

    if (!CheckCorrectIndex (list, ind))
    {
        LOG_REPORT ("Incorrect ind = %lu\n", ind);
        return LIST_GET_VAL_ERR;
    }

    list->data[ind].val = val;

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("EXIT\nFROM: ListChangeVal,"
                       " ind = %lu\n", ind);
        return LIST_GET_VAL_ERR; 
    }

    #endif

    return 0;
}

//======================================================================================

inline static size_t CheckCorrectIndex (const List *list, const size_t ind)
{
    assert (list != nullptr && "list is nullptr");

    #ifdef CHECK_LIST 

    if (ListCheck (list))
    {
        REPORT ("ENTRY\nFROM: CheckCorrectIndex, ind = %lu", ind);
        return CHECK_IND_ERR; 
    }

    #endif


    if (ind == Invalid_ind) return 0;

    if (ind > list->capacity) return 0;

    if (list->data[ind].val == Poison_val) return 0;
    

    //No list re-validation as list items don't change

    return 1;
}

//======================================================================================

static int NotFreeNodeValidity (const List *list)
{
    assert (list != nullptr && "list is nullptr");

    if (CheckNullptr (list->data))
        return 1;

    size_t logical_ind = Invalid_ind;
    size_t counter = 0;
        
    while (counter <= list->size_data)
    {
        if (list->data[logical_ind].prev == Identifier_free_node) return 1;

        if (logical_ind != Invalid_ind && list->data[logical_ind].val == Poison_val) return 1;
        
        logical_ind = list->data[logical_ind].next;
        counter++;            
    }

    if (logical_ind != Invalid_ind) return 1;

    return 0;
}

//======================================================================================

static int FreeNodeValidity (const List *list)
{
    assert (list != nullptr && "list is nullptr");

    if (CheckNullptr (list->data))
        return 1;

    size_t logical_ind = list->free_ptr;
    size_t counter = 1;
        
    while (counter <= list->cnt_free_nodes)
    {   
        if (list->data[logical_ind].prev != Identifier_free_node) return 1;

        if (list->data[logical_ind].val != Poison_val) return 1;

        
        logical_ind = list->data[logical_ind].next;
        counter++;            
    }

    return 0;
}

//======================================================================================

int ListDump_ (const List *list,
                const char* file_name, const char* func_name, int line, const char *format, ...)
{
    assert (list != nullptr && "list is nullptr\n");

    uint64_t err = ListCheck (list);

    FILE *fp_logs = GetLogFilePtr ();

    fprintf (fp_logs, "=================================================\n\n");

    va_list args = {};

    va_start(args, format);
    fprintf (fp_logs, "<h2>");
    vfprintf(fp_logs, format, args);
    fprintf (fp_logs, "</h2>");
    va_end(args);

    fprintf (fp_logs, "REFERENCE:\n");

    if (err) 
        fprintf (fp_logs, "ERROR\nCaused an error in file %s, function %s, line %d\n\n", LOG_VAR);
    else
        fprintf (fp_logs, "OK\nlast call in file %s, function %s, line %d\n\n", LOG_VAR);
    
                     
    if (err) fprintf (fp_logs, "ERR CODE: %lX\n", err);

    DrawListVariable (list, fp_logs);


    #ifdef GRAPH_DUMP

        fprintf (fp_logs, "Logical graph\n");
        ListDrawLogicalGraph (list);
        
        fprintf (fp_logs, "\n\n");

        fprintf (fp_logs, "Physical graph\n");
        ListDrawPhysicalGraph (list);
    
    #else

        for (size_t it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5lu", it);
        fprintf (fp_logs, "\n");
        
        // for (int it = 0; it <= list->capacity; it++)
        //     fprintf (fp_logs, "%5d", list->data[it].val);
        // fprintf (fp_logs, "\n");
        
        for (size_t it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5lu", list->data[it].next);
        fprintf (fp_logs, "\n");

        for (size_t it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5lu", list->data[it].prev);
        fprintf (fp_logs, "\n");
    
    #endif
    
    fprintf (fp_logs, "\n");

    fprintf (fp_logs, "==========================================================\n\n");

    return 0;
}


//======================================================================================

static void DrawListVariable (const List *list, FILE *fpout)
{
    assert (list  != nullptr &&  "list is nullptr\n");
    assert (fpout != nullptr && "fpout is nullptr\n");

    fprintf (fpout, "<body>\n");
    fprintf (fpout, "<table border=\"1\">\n");
    
    fprintf (fpout, "<tr><td> data pointer </td> <td> %p </td></tr>", (char*) list->data);

    fprintf (fpout, "<tr><td> size data </td> <td>  %lu </td></tr>",  list->size_data);
    fprintf (fpout, "<tr><td> capacity </td> <td> %lu </td></tr>",    list->capacity);
    fprintf (fpout, "<tr><td>cnt free nodes</td><td> %lu </td></tr>", list->cnt_free_nodes);

    fprintf (fpout, "<tr><td> head pointer </td> <td>  %lu </td></tr>",  list->head_ptr);
    fprintf (fpout, "<tr><td> tail pointer </td> <td>  %lu </td></tr>",  list->tail_ptr);
    fprintf (fpout, "<tr><td> free pointer </td> <td>  %lu </td></tr>",  list->free_ptr);

    fprintf (fpout, "<tr><td> is_linearized </td> <td>  %d </td></tr>",  list->is_linearized);

    fprintf (fpout, "</table>\n");
    fprintf (fpout, "</body>\n");
   
    return;
}

//======================================================================================

static void PrintListErrors (uint64_t err, FILE *fpout)
{
    assert (fpout != nullptr && "fpout is nullptr\n");

    if (err & CAPACITY_LOWER_SIZE)
        fprintf (fpout, "Capacity is lower than size_data\n");

    if (err & DATA_IS_NULLPTR)
        fprintf (fpout, "Data pointer is nullptr\n");

    if (err & ILLIQUID_HEAD_PTR)
        fprintf (fpout, "Head pointer is incorrect\n");

    if (err & ILLIQUID_TAIL_PTR)
        fprintf (fpout, "Tail pointer is incorrect\n");

    if (err & ILLIQUID_FREE_PTR)
        fprintf (fpout, "Free pointer is incorrect\n");

    if (err & INCORRECT_LINEARIZED)
        fprintf (fpout, "Unknown linearize status\n"); 

    #ifdef LIST_DATA_CHECK

        if (err & DATA_NODE_INCORRECT)
            fprintf (fpout, "Сorrupted not-free nodes\n"); 


        if (err & DATA_FREE_NODE_INCORRECT)
            fprintf (fpout, "Сorrupted free nodes\n"); 
    
    #endif

    fprintf (fpout, "\n\n");

    return;
}

//======================================================================================

static int ListDrawLogicalGraph (const List *list)
{
    assert (list != nullptr && "list is nullptr\n");

    FILE *graph = OpenFilePtr ("graph_img/graph.txt", "w");
    if (CheckNullptr (graph))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "draw list by graphviz failed\n");

    fprintf (graph, "digraph G{\n");
    fprintf (graph, "ranksep = 1.2;\n");
    fprintf (graph, "splines=ortho;\n");
    fprintf (graph, "{\n");

    
    fprintf (graph, "{rank=min;\n");

        if (list->head_ptr != Identifier_free_node)
            fprintf (graph, "node_head [shape = circle, style=filled, color=coral, label=\"HEAD\"];\n");

        if (list->tail_ptr != Identifier_free_node)
            fprintf (graph, "node_tail [shape = circle, style=filled, color=lightgreen, label=\"TAIL\"];\n");
        
        if (list->free_ptr != Identifier_free_node)
            fprintf (graph, "node_free [shape = circle, style=filled, color=plum1, label=\"FREE\"];\n");

    fprintf (graph, "}\n");

    fprintf (graph, "node_head -> node%lu\n", list->head_ptr);
    fprintf (graph, "node_tail -> node%lu\n", list->tail_ptr);
    fprintf (graph, "node_free -> node%lu\n", list->free_ptr);



    static size_t Cnt_graphs = 0;      //<-To display the current list view

    fprintf (graph, "{rank =  same;\n");

    for (size_t it = 0; it <= list->capacity; it++) 
    {
        size_t next = list->data[it].next;
        size_t prev = list->data[it].prev;

        // fprintf (graph, "node%d [style=filled, shape = record, label =  \"{NODE %d | VAL: %d| prev: %lu | next: %lu}}\",", 
        //                 it, it, list->data[it].val, prev, next);

        if (prev != Identifier_free_node)
            fprintf (graph, " fillcolor=lightpink ];\n");
        else
            fprintf (graph, " fillcolor=lightskyblue ];\n");

        if (next != Identifier_free_node)
        {
            fprintf (graph, "node%lu -> node%lu[style=filled, fillcolor=yellow];\n", 
                             it, next);
        }

        if (prev != Identifier_free_node)
        {
            fprintf (graph, "node%lu -> node%lu[style=filled, fillcolor=green];\n", 
                             it, prev);
        }

        fprintf (graph, "\n");
    }


    fprintf(graph, "}\n}\n}\n");
    fclose(graph);

    char command_buffer[Init_buffer] = {0};
    sprintf(command_buffer, "dot -Tpng graph_img/graph.txt -o graph_img/picture_logical%lu.png", Cnt_graphs);

    if (system(command_buffer))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "draw list by graphviz failed\n");
 

    FILE *fp_logs = GetLogFilePtr ();
    if (CheckNullptr (fp_logs))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "logs file pointer is nullptr\n");

    fprintf (fp_logs, "<img src= graph_img/picture_logical%lu.png />\n", Cnt_graphs);
                                
    Cnt_graphs++;
    return 0;
}

//======================================================================================

static uint64_t ListCheck (const List *list)
{
    assert (list != nullptr && "list is nullptr");

    uint64_t err = 0;

    if (list->capacity  < list->size_data)    err |= CAPACITY_LOWER_SIZE;

    if (CheckNullptr (list->data))           err |= DATA_IS_NULLPTR;

    if (list->head_ptr == Poison_ptr                     ||
        list->data[list->tail_ptr].next != Invalid_ind   ) err |= ILLIQUID_HEAD_PTR;

    if (list->tail_ptr == Poison_ptr                     ||
        list->data[list->tail_ptr].next != Invalid_ind   ) err |= ILLIQUID_TAIL_PTR;

    if (list->free_ptr == Poison_ptr) err |= ILLIQUID_FREE_PTR;

    if ((list->is_linearized != 0) && (list->is_linearized != 1)) err |= INCORRECT_LINEARIZED; 

    #ifdef LIST_DATA_CHECK

        if (NotFreeNodeValidity (list))   err |= DATA_NODE_INCORRECT;

        if (FreeNodeValidity (list))      err |= DATA_FREE_NODE_INCORRECT;
    
    #endif

    return err;
}

//======================================================================================