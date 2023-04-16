#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "list.h"

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"


static int          InitListData        (List *list);

static int          ListResize          (List *list);

static int          ListRecalloc        (List *list, int ver_resize);

static void         InitNode            (Node *list_elem, elem_t val, int next, int prev);

static int          CheckCorrectIndex   (const List *list, const int ind);

static int          NotFreeNodeValidity (const List *list);

static int          FreeNodeValidity    (const List *list);

static uint64_t     LIstCheck           (const List *list);


static int      ListDrawLogicalGraph    (const List *list);

static int      ListDrawPhysicalGraph   (const List *list);

static void     DrawListVariable        (const List *list, FILE *fpout);

static void     PrintListErrors         (uint64_t err, FILE *fpout);

#define REPORT(...)                                         \
    {                                                       \
        ListDump (list, __VA_ARGS__);                       \
        ERR_REPORT (__VA_ARGS__);                           \
                                                            \
    }while (0)
                                    
//======================================================================================

int ListCtor (List *list, long capacity)
{
    assert (list != nullptr && "list is nullptr");

    if (capacity <= 0)
        return PROCESS_ERROR (LIST_CTOR_ERR, "Incorrectly entered capacity values: %d\n", capacity);

    list->tail_ptr = Dummy_element;
    list->head_ptr = Dummy_element;
    list->free_ptr = 1;

    list->is_linearized = 1;

    list->size_data      = 0;
    list->capacity       = capacity;
    list->cnt_free_nodes = 0;

    list->data = (Node*) calloc (capacity + 1, sizeof (Node));

    if (CheckNullptr(list->data))
        return PROCESS_ERROR(LIST_CTOR_ERR, "Memory allocation error\n");
    
    InitNode (&list->data[0], Poison_val, Dummy_element, Dummy_element);

    if (InitListData(list))
        return PROCESS_ERROR (LIST_CTOR_ERR, "Node initialization error\n");

    return 0;
}

//======================================================================================

int ListDtor (List *list)
{
    assert (list != nullptr && "list ptr is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListDtor\n");
        return LIST_DTOR_ERR;
    }

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

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: InitListData\n");
        return DATA_INIT_ERR;
    }

    int Last_used_node = MAX (list->tail_ptr, list->head_ptr);


    for (int ip = Last_used_node + 1; ip < list->capacity; ip++) 
        InitNode (list->data + ip, Poison_val, ip + 1, Identifier_free_node);

    InitNode (list->data + list->capacity, Poison_val, Identifier_free_node, Identifier_free_node);


    list->cnt_free_nodes = list->capacity - list->size_data;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: InitListData\n");
        return DATA_INIT_ERR;
    }

    return 0;
}

//======================================================================================

static void InitNode (Node *list_elem, elem_t val, int next, int prev)
{
    assert (list_elem != nullptr && "list_elem ptr is nullptr");

    list_elem->val  = val;
    list_elem->next = next;
    list_elem->prev = prev; 

    return;
}

//======================================================================================

int ListInsertPrevInd (List *list, const int ind, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertPrevInd,"
                        " ind = %d, val = %d\n", ind, val);
        return LIST_INSERT_ERR;
    }
    
    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");

    if (!CheckCorrectIndex (list, ind) && ind != Dummy_element)
        return PROCESS_ERROR (LIST_INSERT_ERR, "Incorrect ind = %d\n", ind);
      

    
    if (list->free_ptr == Dummy_element)
        return PROCESS_ERROR (LIST_INSERT_ERR, "No free space in list\n");


    if (list->data[ind].prev == Identifier_free_node)
    {
        return PROCESS_ERROR (LIST_INSERT_ERR, "There is nothing at this pointer: %d.\n" 
                                        "You can only add an element before initialized elements\n", ind);
    }

        
    if (list->head_ptr != ind && list->tail_ptr != ind)
        list->is_linearized = 0;

    
    int  prev_ptr      = ind;
    int  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    int next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Dummy_element].next;
    list->tail_ptr = list->data[Dummy_element].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertPrevInd,"
                        " ind = %d, val = %d\n", ind, val);
        return LIST_INSERT_ERR;
    }

    return cur_free_ptr;
}

//======================================================================================

int ListInsertFront (List *list, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertFront %d\n", val);
        return LIST_INSERT_ERR;
    }
    
    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
        
    if (list->size_data != 0)
        list->is_linearized = 0;

    
    int  prev_ptr      = Dummy_element;
    int  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    int next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Dummy_element].next;
    list->tail_ptr = list->data[Dummy_element].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertFront %d\n", val);
        return LIST_INSERT_ERR;
    }

    return cur_free_ptr;
}

//======================================================================================

int ListInsertBack (List *list, const elem_t val) 
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListInsertBack %d\n", val);
        return LIST_INSERT_ERR;
    }

    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
    
    
    int  prev_ptr      = list->tail_ptr;
    int  cur_free_ptr  = list->free_ptr;

    list->free_ptr = list->data[cur_free_ptr].next;  
    
    InitNode (list->data + cur_free_ptr, 
               val, list->data[prev_ptr].next, prev_ptr);
    
    int next_ptr = list->data[cur_free_ptr].next;

    list->data[next_ptr].prev = cur_free_ptr;
    list->data[prev_ptr].next = cur_free_ptr;
    
    list->head_ptr = list->data[Dummy_element].next;
    list->tail_ptr = list->data[Dummy_element].prev;

    list->size_data++;
    list->cnt_free_nodes--;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListInsertBack %d\n", val);
        return LIST_INSERT_ERR;
    }

    return cur_free_ptr;
}

//======================================================================================

int ListErase (List *list, const int ind) 
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListErase, ind = %d\n", ind);
        return LIST_ERASE_ERR;
    }   
    
    if (!CheckCorrectIndex (list, ind))
        return PROCESS_ERROR (LIST_ERASE_ERR, "Incorrect ind = %d\n", ind);


    if (list->data[ind].prev == Identifier_free_node)
    {
        LOG_REPORT ("There is nothing at this pointer: %d.\n" 
                    "You cannot free a previously freed node\n", ind);
        return LIST_ERASE_ERR;
    }


    if (list->head_ptr != ind && list->tail_ptr != ind)
        list->is_linearized = 0;
    

    int ver_resize = ListResize (list);
    if (ListRecalloc (list, ver_resize))
        return PROCESS_ERROR (LIST_INSERT_ERR, "Recalloc error\n");
    

    int  cur_ptr   = ind;
    int  prev_ptr  = list->data[ind].prev;
    int  next_ptr  = list->data[ind].next;

    list->data[prev_ptr].next = next_ptr;
    list->data[next_ptr].prev = prev_ptr;

    InitNode (list->data + cur_ptr, 
              Poison_val, list->free_ptr, Identifier_free_node);

    list->free_ptr = cur_ptr;

    list->head_ptr = list->data[Dummy_element].next;
    list->tail_ptr = list->data[Dummy_element].prev;

    list->size_data--;
    list->cnt_free_nodes++;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListErase exit, ind = %d\n", ind);
        return LIST_ERASE_ERR;
    }  

    return 0;
}

//======================================================================================

static int ListResize (List *list)
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListResize\n");
        return PTR_OUT_OF_MEMORY_ERR;
    }   

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

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListResize\n");
        return PTR_OUT_OF_MEMORY_ERR;
    }  

    return 0;
}

//======================================================================================

static int ListRecalloc (List *list, int resize_status)
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListRecalloc\n");
        return LIST_RECALLOC_ERR;
    } 

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
     

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListRecalloc\n");
        return LIST_RECALLOC_ERR;
    } 

    return 0;
}

//======================================================================================

int ListLinearize (List *list)
{
    assert (list != nullptr && "list is nullptr\n");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListLinearize\n");
        return LIST_LINEARIZE_ERR;
    } 

    if (list->is_linearized == 1) 
        return 0;

    Node* new_data = (Node*) calloc (list->capacity + 1, sizeof (Node));

    if (CheckNullptr (new_data))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Linearize error, new data memory allocation error\n");

    int logical_ind = list->head_ptr;

    InitNode (new_data, Poison_val, 1l, (int)list->size_data);

    int counter = 1;

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
               list->data[logical_ind].val, Dummy_element, counter - 1);


    free (list->data);

    list->cnt_free_nodes = 0;

    list->data = new_data;
    list->head_ptr = list->data[Dummy_element].next;
    list->tail_ptr = list->data[Dummy_element].prev;

    if (InitListData (list))
        return PROCESS_ERROR (LIST_LINEARIZE_ERR, "List data initialization error\n");

    list->is_linearized = 1;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListLinearize\n");
        return LIST_LINEARIZE_ERR;
    }

    return 0;;
}

//======================================================================================

int GetLogicalIndex (const List *list, const int ind)
{
    assert (list != nullptr && "list is nullptr\n");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: GetLogicalIndex, ind = %d\n", ind);
        return GET_LOGICAL_PTR_ERR;
    }   

    if (!CheckCorrectIndex (list, ind) && 
         list->data[ind].prev != Identifier_free_node)
    {
        return PROCESS_ERROR (GET_LOGICAL_PTR_ERR, "Incorrect ind = %d\n", ind);
    }

    if (ind > list->size_data)
        return PROCESS_ERROR (GET_LOGICAL_PTR_ERR, "Number of elements of the requested index.\nind = %d\n", ind);
       


    if (list->is_linearized)
    {
        return list->head_ptr + ind - 1;
    }

    else
    {   
        int logical_ind = list->head_ptr;
        int counter = 1;

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

elem_t ListGetVal (const List *list, const int ind)
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListGetVal, ind = %d\n", ind);
        return Poison_val; 
    }

    if (!CheckCorrectIndex (list, ind))
    {
        PROCESS_ERROR (GET_VAL_ERR, "Incorrect ind = %d\n", ind);
        return Poison_val;
    }

    //No list re-validation as list items don't change

    return list->data[ind].val;
}

//======================================================================================

int ListChangeVal (const List *list, const int ind, const elem_t val)
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: ListChangeVal,"
                       " ind = %d, val = %d\n", ind, val);
        return LIST_GET_VAL_ERR; 
    } 

    if (!CheckCorrectIndex (list, ind))
    {
        LOG_REPORT ("Incorrect ind = %d\n", ind);
        return LIST_GET_VAL_ERR;
    }

    list->data[ind].val = val;

    if (LIstCheck (list))
    {
        REPORT ("EXIT\nFROM: ListChangeVal,"
                       " ind = %d, val = %d\n", ind, val);
        return LIST_GET_VAL_ERR; 
    }

    return 0;
}

//======================================================================================

static int CheckCorrectIndex (const List *list, const int ind)
{
    assert (list != nullptr && "list is nullptr");

    if (LIstCheck (list))
    {
        REPORT ("ENTRY\nFROM: CheckCorrectIndex, ind = %d", ind);
        return CHECK_IND_ERR; 
    }

    if (ind < 0) return 0;

    if (ind == 0) return 0;

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

    int logical_ind = Dummy_element;
    int counter = 0;
        
    while (counter <= list->size_data)
    {
        if (logical_ind < 0) return 1;

        if (list->data[logical_ind].prev == Identifier_free_node) return 1;

        if (logical_ind != Dummy_element && list->data[logical_ind].val == Poison_val) return 1;
        
        logical_ind = list->data[logical_ind].next;
        counter++;            
    }

    if (logical_ind != Dummy_element) return 1;

    return 0;
}

//======================================================================================

static int FreeNodeValidity (const List *list)
{
    assert (list != nullptr && "list is nullptr");

    if (CheckNullptr (list->data))
        return 1;

    int logical_ind = list->free_ptr;
    int counter = 1;
        
    while (counter <= list->cnt_free_nodes)
    {
        if (logical_ind < 0) return 1;
        
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

    uint64_t err = LIstCheck (list);

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

    PrintListErrors (err, fp_logs);

    #ifdef GRAPH_DUMP

        fprintf (fp_logs, "Logical graph\n");
        ListDrawLogicalGraph (list);
        
        fprintf (fp_logs, "\n\n");

        fprintf (fp_logs, "Physical graph\n");
        ListDrawPhysicalGraph (list);
    
    #else

        for (int it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5d", it);
        fprintf (fp_logs, "\n");
        
        // for (int it = 0; it <= list->capacity; it++)
        //     fprintf (fp_logs, "%5d", list->data[it].val);
        // fprintf (fp_logs, "\n");
        
        for (int it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5d", list->data[it].next);
        fprintf (fp_logs, "\n");

        for (int it = 0; it <= list->capacity; it++)
            fprintf (fp_logs, "%5d", list->data[it].prev);
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

    fprintf (fpout, "<tr><td> size data </td> <td>  %ld </td></tr>",  list->size_data);
    fprintf (fpout, "<tr><td> capacity </td> <td> %ld </td></tr>",    list->capacity);
    fprintf (fpout, "<tr><td>cnt free nodes</td><td> %ld </td></tr>", list->cnt_free_nodes);

    fprintf (fpout, "<tr><td> head pointer </td> <td>  %d </td></tr>",  list->head_ptr);
    fprintf (fpout, "<tr><td> tail pointer </td> <td>  %d </td></tr>",  list->tail_ptr);
    fprintf (fpout, "<tr><td> free pointer </td> <td>  %d </td></tr>",  list->free_ptr);

    fprintf (fpout, "<tr><td> is_linearized </td> <td>  %d </td></tr>",  list->is_linearized);

    fprintf (fpout, "</table>\n");
    fprintf (fpout, "</body>\n");
   
    return;
}

//======================================================================================

static void PrintListErrors (uint64_t err, FILE *fpout)
{
    assert (fpout != nullptr && "fpout is nullptr\n");

    if (err & NEGATIVE_SIZE)
        fprintf (fpout, "Size_data is negative number\n");

    if (err & NEGATIVE_CAPACITY)
        fprintf (fpout, "Capacity is negative number\n");

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

        if (list->head_ptr >= 0)
            fprintf (graph, "node_head [shape = circle, style=filled, color=coral, label=\"HEAD\"];\n");


        if (list->tail_ptr >= 0)
            fprintf (graph, "node_tail [shape = circle, style=filled, color=lightgreen, label=\"TAIL\"];\n");
        
        if (list->free_ptr >= 0)
            fprintf (graph, "node_free [shape = circle, style=filled, color=plum1, label=\"FREE\"];\n");

    fprintf (graph, "}\n");

    fprintf (graph, "node_head -> node%d\n", list->head_ptr);
    fprintf (graph, "node_tail -> node%d\n", list->tail_ptr);
    fprintf (graph, "node_free -> node%d\n", list->free_ptr);



    static int Cnt_graphs = 0;      //<-To display the current list view

    fprintf (graph, "{rank =  same;\n");

    for (int counter = 0; counter <= list->capacity; counter++) 
    {
        int next = list->data[counter].next;
        int prev = list->data[counter].prev;

        // fprintf (graph, "node%d [style=filled, shape = record, label =  \"{NODE %d | VAL: %d| prev: %d | next: %d}}\",", 
        //                 counter, counter, list->data[counter].val, prev, next);

        if (prev != -1)
            fprintf (graph, " fillcolor=lightpink ];\n");
        else
            fprintf (graph, " fillcolor=lightskyblue ];\n");

        if (next != -1)
        {
            fprintf (graph, "node%d -> node%d[style=filled, fillcolor=yellow];\n", 
                             counter, next);
        }

        if (prev != -1)
        {
            fprintf (graph, "node%d -> node%d[style=filled, fillcolor=green];\n", 
                             counter, prev);
        }

        fprintf (graph, "\n");
    }


    fprintf(graph, "}\n}\n}\n");
    fclose(graph);

    char command_buffer[Init_buffer] = {0};
    sprintf(command_buffer, "dot -Tpng graph_img/graph.txt -o graph_img/picture_logical%d.png", Cnt_graphs);

    if (system(command_buffer))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "draw list by graphviz failed\n");
 

    FILE *fp_logs = GetLogFilePtr ();
    if (CheckNullptr (fp_logs))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "logs file pointer is nullptr\n");

    fprintf (fp_logs, "<img src= graph_img/picture_logical%d.png />\n", Cnt_graphs);
                                
    Cnt_graphs++;
    return 0;
}

//======================================================================================

static int ListDrawPhysicalGraph (const List *list)
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

        if (list->head_ptr >= 0)
            fprintf (graph, "node_head [shape = circle, style=filled, color=coral, label=\"HEAD\"];\n");


        if (list->tail_ptr >= 0)
            fprintf (graph, "node_tail [shape = circle, style=filled, color=lightgreen, label=\"TAIL\"];\n");
        
        if (list->free_ptr >= 0)
            fprintf (graph, "node_free [shape = circle, style=filled, color=plum1, label=\"FREE\"];\n");

    fprintf (graph, "}\n");

    fprintf (graph, "node_head -> node%d\n", list->head_ptr);
    fprintf (graph, "node_tail -> node%d\n", list->tail_ptr);
    fprintf (graph, "node_free -> node%d\n", list->free_ptr);



    static int Cnt_graphs = 0;      //<-To display the current list view

    fprintf (graph, "{rank =  same;\n");

    for (int counter = 0; counter <= list->capacity; counter++) 
    {
        int next = list->data[counter].next;
        int prev = list->data[counter].prev;

        // fprintf (graph, "node%d [style=filled, shape = record, label =  \"{NODE %d | VAL: %d| prev: %d | next: %d}}\",", 
        //                 counter, counter, list->data[counter].val, prev, next);

        if (prev != -1)
            fprintf (graph, " fillcolor=lightpink ];\n");
        else
            fprintf (graph, " fillcolor=lightskyblue ];\n");

        if (next != -1)
        {
            fprintf (graph, "node%d -> node%d[style=filled, fillcolor=yellow, weight = 0];\n", 
                             counter, next);
        }

        if (prev != -1)
        {
            fprintf (graph, "node%d -> node%d[style=filled, fillcolor=green, weight = 0];\n", 
                             counter, prev);
        }

        fprintf (graph, "\n");
    
        if (counter != list->capacity)
        {
              fprintf (graph, "node%d -> node%d[style = invis, weight = 10000];\n", 
                             counter, counter + 1);
        }

        fprintf (graph, "\n"); 
    }


    fprintf(graph, "}\n}\n}\n");
    fclose(graph);

    char command_buffer[Init_buffer] = {0};
    sprintf(command_buffer, "dot -Tpng graph_img/graph.txt -o graph_img/picture_physical%d.png", Cnt_graphs);

    if (system(command_buffer))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "draw list by graphviz failed\n");
 

    FILE *fp_logs = GetLogFilePtr ();
    if (CheckNullptr (fp_logs))
        return PROCESS_ERROR(LIST_DRAW_GRAPH_ERR, "logs file pointer is nullptr\n");

    fprintf (fp_logs, "<img src= graph_img/picture_physical%d.png />\n", Cnt_graphs);
                                
    Cnt_graphs++;
    return 0;
}

//======================================================================================

static uint64_t LIstCheck (const List *list)
{
    assert (list != nullptr && "list is nullptr");

    uint64_t err = 0;

    if (list->size_data < 0) err |= NEGATIVE_SIZE;
    if (list->capacity  < 0) err |= NEGATIVE_CAPACITY;


    if (list->capacity  < list->size_data)    err |= CAPACITY_LOWER_SIZE;

    if (CheckNullptr (list->data))           err |= DATA_IS_NULLPTR;

    if (list->head_ptr == Poison_ptr                     ||
        list->head_ptr < 0                               ||  
        list->data[list->tail_ptr].next != Dummy_element   ) err |= ILLIQUID_HEAD_PTR;

    if (list->tail_ptr == Poison_ptr                     ||
        list->tail_ptr < 0                               ||  
        list->data[list->tail_ptr].next != Dummy_element   ) err |= ILLIQUID_TAIL_PTR;

    if (list->free_ptr <  0             || 
        list->free_ptr > list->capacity ||  
        list->free_ptr == Poison_ptr      ) err |= ILLIQUID_FREE_PTR;

    if ((list->is_linearized != 0) && (list->is_linearized != 1)) err |= INCORRECT_LINEARIZED; 

    #ifdef LIST_DATA_CHECK

        if (NotFreeNodeValidity (list))   err |= DATA_NODE_INCORRECT;

        if (FreeNodeValidity (list))      err |= DATA_FREE_NODE_INCORRECT;
    
    #endif

    return err;
}

//======================================================================================