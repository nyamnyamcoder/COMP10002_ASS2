/* Program to generate text based on the context provided by input prompts.

  Skeleton program written by Artem Polyvyanyy, http://polyvyanyy.com/,
  September 2023, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.
  All included code is (c) Copyright University of Melbourne, 2023.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,   
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Miyuki Ishige 1379202
  Dated: September 25th, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n"   // stage delimiter
#define MDELIM "-------------------------------------\n"    // delimiter of -'s
#define THEEND "==THE END============================\n"    // end message
#define NOSFMT "Number of statements: %d\n"                 // no. of statements
#define NOCFMT "Number of characters: %d\n"                 // no. of chars
#define NPSFMT "Number of states: %d\n"                     // no. of states
#define TFQFMT "Total frequency: %d\n"                      // total frequency

#define CRTRNC '\r'                 // carriage return character
#define NEWLNC '\n'                 // new line character
#define STRNUL '\0'                 // NULL character at the end of string
#define NEWLNA 10                   // ASCII value of '\n'
#define PERIDA 46                   // ASCII value of "."
#define ZEROA  48                   // ASCII value of "0"

#define STAGE0 0                    // Stage 0 indicator
#define STAGE1 1                    // Stage 1 indicator
#define STAGE2 2                    // Stage 2 indicator

#define STRSIZE 2                   // Size of strings made in stage 0
#define NELLIP 3                    // Default number of ellipses
#define GOTOSTG2 0                  // Indicator to go to stage 2
#define ENDAUTOM 0                  // Indicator to end this automaton program
#define CHLIMIT 37                  // Limit of characters that can be printed
                                    // per line
#define RSTZERO 0                   // Reset incremented variables to zero

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef struct state state_t;   // a state in an automaton
typedef struct node  node_t;    // a node in a linked list
typedef struct stacknode stacknode_t; // a node in the stack for DFS

struct node {                   // a node in a linked list of transitions has
    char*           str;        // ... a transition string
    state_t*        state;      // ... the state reached via the string, and
    node_t*         next;       // ... a link to the next node in the list.
};

typedef struct {                // a linked list consists of
    node_t*         head;       // ... a pointer to the first node and
    node_t*         tail;       // ... a pointer to the last node in the list.
} list_t;

struct state {                  // a state in an automaton is characterized by
    unsigned int    id;         // ... an identifier,
    unsigned int    freq;       // ... frequency of traversal,
    list_t*         outputs;    // ... a list of output states.
};

typedef struct {                // an automaton consists of
    state_t*        ini;        // ... the initial state,
    unsigned int    nid;        // ... the total number of states, and
    unsigned int    nfreq;      // ... the total frequency of states.
} automaton_t;

// The structure for stacknode is referenced from: 
// https://iq.opengenus.org/depth-first-search/
struct stacknode {                  // a node in an stack consists of
    state_t* data;                  // ... a pointer to the state and
    stacknode_t* next;              // ... a pointer to one bottom stacknode
};

typedef struct {                    // a stack consists of
    stacknode_t* top;               // ... the stack node at the very top
} mystack_t;

/* USEFUL FUNCTIONS ----------------------------------------------------------*/
automaton_t* do_stage0(automaton_t* automaton);
void do_stage1(automaton_t* automaton);
void do_stage2(automaton_t* automaton);
int mygetchar(void);            
state_t* make_empty_state(void);
list_t* make_empty_list(void);
node_t* make_empty_node(void);
void make_new_set(state_t** state_ptr, int id);
char* make_empty_str(int length);
void curr_set(state_t* prev_state, state_t** curr_state, 
list_t** curr_list, node_t** curr_node);
int count_nodes(node_t* tmp_node);
void print_ellipses(int nellipse);
void generate_text(state_t* curr_state, list_t* curr_list, 
node_t* curr_node, int pos, int printed);
int print_str(char* string, int pos, int initial);
int print_wo_newln(int input_ascii, int nellipse);
node_t* cmp_state(node_t* tmp_node, node_t* tmp2_node);
void sort_node(list_t** curr_list);
state_t** depth_first_search(automaton_t* automaton);
void poptop_stack(mystack_t** stack);
void pushtop_stack(mystack_t* stack, state_t* curr_state);
void free_automaton(state_t* ini, automaton_t* automaton);
void recursive_free_set(state_t* state);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(int argc, char *argv[]) {
    //STAGE 0
    printf(SDELIM, STAGE0);
    // create an automaton
    automaton_t* automaton = NULL;
    automaton = do_stage0(automaton); 
    //STAGE 1
    printf(SDELIM, STAGE1);
    do_stage1(automaton);
    //STAGE 2
    printf(SDELIM, STAGE2);
    do_stage2(automaton);
    // FREE THE AUTOMATON
    free_automaton(automaton->ini, automaton);
    printf(THEEND);
    return EXIT_SUCCESS;        
    // algorithms are fun!!!
}
// Function to perform requirements of STAGE 0;
// returns the pointer to the automaton created.
automaton_t* do_stage0(automaton_t* automaton) {
    // initialize an automaton
    automaton = (automaton_t*)malloc(sizeof(*automaton));
    assert(automaton!=NULL);
    automaton->ini = NULL;
    // character to be stored in each node
    char ch;
    // id of each state
    int id=0;
    // pointers to traverse the automaton
    state_t* curr_state;
    list_t* curr_list;
    node_t* curr_node;
    make_new_set(&(automaton->ini), id);
    curr_set(automaton->ini, &curr_state, &curr_list, &curr_node);
    while(scanf("%c", &ch) == 1){ 
        if (ch == NEWLNC){
            if (scanf("%c", &ch) == 1){
                // NEWLNC twice means finish STAGE 0
                if (ch == NEWLNC){
                    break;
                } else {
                    // start over;
                    // bring the curr_state pointer back to the initial state
                    curr_set(automaton->ini, &curr_state, 
                    &curr_list, &curr_node);
                }
            }
        }
        char* new_str;
        new_str = make_empty_str(STRSIZE);
        new_str[0] = ch;
        new_str[1] = STRNUL;  
        // an arc would leave the current state so increase the frequency by 1
        curr_state->freq++;
        // increase the total frequency of states in automaton as well
        automaton->nfreq++;
        // Node has no string associated, add new_str
        if (curr_node->str == NULL){
            curr_node->str = new_str;
            id++;
            make_new_set(&(curr_node->state), id);
            curr_set(curr_node->state, &curr_state, &curr_list, &curr_node);
        // if the string is the same, just need to point to the next state
        } else if (strcmp(new_str, curr_node->str) == 0){
            curr_set(curr_node->state, &curr_state, &curr_list, &curr_node);
        } else {
            node_t* tmp_node = NULL;
            tmp_node = curr_node;
            // variables as guard conditions for following while loop and if
            int different=1, status=1;
            // traverse the possible outputs from current state
            while (status){
                // this is the last node, no node left to traverse
                if (tmp_node->next == NULL){
                    status=0;
                } 
                // same string already exist at a node
                if (strcmp(new_str, tmp_node->str) == 0){
                    curr_set(tmp_node->state, &curr_state, 
                    &curr_list, &curr_node);
                    // new_string and the string at tmp_node is the same
                    different=0;
                    break;
                // not the last node, and same string not found
                } else if (status == 1){
                    tmp_node = tmp_node->next;
                }
            }
            // if the new_str is not in the possible outcomes
            if (different){
                // create a new node
                curr_list->tail = make_empty_node();
                tmp_node->next = curr_list->tail;
                tmp_node->next->str = new_str;
                //make new set emerging from the new node
                id++;
                make_new_set(&(tmp_node->next->state), id);
                // point to the next state that can be reached from the new node
                curr_set(tmp_node->next->state, &curr_state, 
                &curr_list, &curr_node);
            }
        }
    }
    // print out the reuslt
    printf(NOSFMT, automaton->ini->freq);
    printf(NOCFMT, automaton->nfreq);
    // since id of states is counted from 0, 
    // the total number of states is id + 1
    int total_states = id+1;
    printf(NPSFMT, total_states);
    return automaton;
}
// Function to perform requirements of STAGE 1.
void do_stage1(automaton_t* automaton){
    // prev_ascii is the guard condition of while loop,
    // initially set as 1 to start the while loop
    int input_ascii, prev_ascii=1, printed=0, pos=0;
    // pointers to traverse the automaton
    state_t* curr_state; list_t* curr_list;
    node_t* curr_node; node_t* tmp_node;
    curr_set(automaton->ini, &curr_state, &curr_list, &curr_node);
    while(prev_ascii) {
        int i, count_node, node_passed=0; 
        input_ascii=mygetchar();
        // replay through the automaton
        tmp_node = curr_node;
        count_node = count_nodes(tmp_node);
        for (i=0; i < count_node; i++){
            char* curr_str = tmp_node->str;
            // check for the special cases
            if (input_ascii == NEWLNA || input_ascii == EOF){
                // break the loop if there are consecutive NEWLNC
                if (prev_ascii == NEWLNA){
                    prev_ascii = GOTOSTG2;
                    break;
                } else {
                    if (input_ascii == NEWLNA){
                        // keep track in case of consecutive NEWLNC 
                        prev_ascii = NEWLNA;
                    } else if (input_ascii == EOF){
                        // if input_ascii is EOF, break the while loop
                        prev_ascii = ENDAUTOM;
                    }
                    generate_text(curr_state, curr_list, 
                    curr_node, pos, printed);
                    // start replaying the automaton
                    curr_set(automaton->ini, &curr_state, 
                    &curr_list, &curr_node);
                    printed=RSTZERO;
                    pos=RSTZERO;
                    break;
                }
            // no text generation, replaying only
            } else if (curr_str == NULL){
                curr_set(automaton->ini, &curr_state, &curr_list, &curr_node);
                printed=print_wo_newln(input_ascii, CHLIMIT-printed);
                pos=RSTZERO;
                break;
            }
            int str_len = strlen(curr_node->str);
            int curr_ascii = curr_str[pos];
            if (input_ascii == curr_ascii) {
                prev_ascii=input_ascii;
                // if this input will reach CHLIMIT printed characters
                if (printed == CHLIMIT-1){
                    printed=print_wo_newln(input_ascii, CHLIMIT-printed);
                    curr_set(automaton->ini, &curr_state, 
                    &curr_list, &curr_node);
                    pos=RSTZERO;
                    prev_ascii = NEWLNA;
                    break;
                } else {
                    printf("%c", input_ascii);
                    printed++;
                    // if there are more than one character in the curr_str, 
                    // check the next character
                    if (pos != str_len-1){
                        pos++; 
                        i--;
                        break;
                    } else {
                        pos=RSTZERO;
                        curr_set(tmp_node->state, &curr_state, 
                        &curr_list, &curr_node);
                        break;
                    }
                }
            // if current character is not same as input character
            } else if (input_ascii != curr_ascii){
                prev_ascii=input_ascii;
                node_passed++;
                // if all nodes don't have the same character as input
                if (node_passed == count_node){
                    printed = print_wo_newln(input_ascii, CHLIMIT-printed);
                    curr_set(automaton->ini, &curr_state, 
                    &curr_list, &curr_node);
                    pos=RSTZERO;
                    break;
                } else if (tmp_node->next != NULL){
                    tmp_node = tmp_node->next;
                }
            }    
        }
    }
    return;
}
// Function to perform requirements of STAGE 2.
void do_stage2(automaton_t* automaton){
    int i, j, k;
    int ncompression=0, input_ascii, count_node_x, count_node_y;
    while ((input_ascii=mygetchar()) != NEWLNA){
        // convert input integer characters into an integer
        ncompression = 10 * ncompression + (input_ascii - ZEROA);   
    }
    // perform compression on the automaton for ncompression times
    for (i=0; i < ncompression; i++){
        // find depth first order of states in automaton
        state_t** dfo = depth_first_search(automaton);
        state_t* state_x = NULL, * state_y = NULL;
        // search for the eligible states for compression
        for (j=0; j < (automaton->nid)-1; j++){
            count_node_x = count_nodes(dfo[j]->outputs->head);
            // if found
            if (count_node_x == 1 && dfo[j]->outputs->head->str != NULL 
                && dfo[j+1]->outputs->head->str != NULL) {
                state_x = dfo[j];
                state_y = dfo[j+1];
                break;
            }
        }
        // if not found, no compression occurs
        if (state_x == NULL && state_y == NULL){
            break;
        }
        // do compression at eligible states
        list_t* list_x = state_x->outputs; 
        node_t* node_x = list_x->head;
        node_t* curr_node_y = state_y->outputs->head;
        node_t* tmp_node, * old_node, * new_node;
        int total_strlen;
        // create a copy of string from node_x
        char* copystr_x = malloc(strlen(node_x->str)+1);
        strcpy(copystr_x, node_x->str); 
        // string from node_x copied, free the node_x
        free(node_x);
        node_x = NULL;
        // create new nodes for as many time as number of nodes from state_y
        count_node_y = count_nodes(state_y->outputs->head);
        for (k=0; k < count_node_y; k++){
            new_node = make_empty_node();
            total_strlen = strlen(copystr_x) + strlen(curr_node_y->str);
            new_node->str = make_empty_str(total_strlen+1);
            strcpy(new_node->str, copystr_x);
            // concatenate the strings from node_x and curr_node_y
            strcat(new_node->str, curr_node_y->str);
            // connect the node to state coming out from curr_node_y
            new_node->state = curr_node_y->state;
            // free the node coming out from state_y
            old_node = curr_node_y;
            if (curr_node_y->next != NULL){
                curr_node_y = curr_node_y->next;
            } 
            free(old_node);
            old_node = NULL;
            // connect the head and tail nodes to list_x
            if (k==0){
                list_x->head = new_node;
            } else if (k==count_node_y-1){
                list_x->tail = new_node;
            }
            // link the newly created neighbouring nodes
            if (k!=0){
                tmp_node->next = new_node;
            }
            tmp_node = new_node;
        }
        // because old_node is freed, 
        // update the frequency and number of states in the automaton
        automaton->nfreq -= state_y->freq;
        automaton->nid -= 1;
        // free the state_y and dfo array
        free(state_y);
        state_y = NULL;
        free(dfo);
        dfo = NULL;
    }
    printf(NPSFMT, automaton->nid);
    printf(TFQFMT, automaton->nfreq);
    printf(MDELIM);
    // process the text inputs and geenrate appropriate texts
    do_stage1(automaton);
    return;
}
/* USEFUL FUNCTIONS ----------------------------------------------------------*/

// An improved version of getchar(); skips carriage return characters.
// NB: Adapted version of the mygetchar() function by Alistair Moffat
int mygetchar() {
    int c;
    while ((c=getchar())==CRTRNC);
    return c;
}
// Malloc an empty state;
// returns the pointer to the allocated memory. 
state_t* make_empty_state(void){
    state_t* state;
    state = (state_t*)malloc(sizeof(*state));
    assert(state != NULL);
    state->outputs = NULL;
    return state;
}
// Malloc an empty list;
// returns the pointer to the allocated memory. 
list_t* make_empty_list(void){
    list_t* list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list != NULL);
    list->head = NULL;
    list->tail = NULL;
    return list;
}
// Malloc an empty node;
// returns the pointer to the allocated memory. 
node_t* make_empty_node(void){
    node_t* node;
    node = (node_t*)malloc(sizeof(*node));
    assert(node != NULL);
    node->str = NULL;
    node->state = NULL;
    node->next = NULL;
    return node;
}
// Make new set of state, node, and list 
// that emerge from the state_ptr passed in.
void make_new_set(state_t** state_ptr, int id){
    // make new state, pointed by state_ptr
    *state_ptr = make_empty_state();
    (*state_ptr)->id = id;
    // no arc passed in & out yet, frequency of state is 0
    (*state_ptr)->freq = 0;
    // make new list, poitned by list_t* outputs
    (*state_ptr)->outputs = make_empty_list();
    // make new node, pointed by list_t head & tail
    (*state_ptr)->outputs->head = make_empty_node();
    (*state_ptr)->outputs->tail = (*state_ptr)->outputs->head;
    return;
}
// Malloc an empty string for desired length;
// returns the pointer to the allocated memory.
char* make_empty_str(int length){
    char* str;
    str = (char*)malloc(length * sizeof(char));
    assert(str != NULL);
    return str;
}
// Update curr_state, curr_node, curr_list to point to next state,
// that can be reached from the state_ptr passed in.
void curr_set(state_t* state_ptr, state_t** curr_state, 
list_t** curr_list, node_t** curr_node){
    *curr_state = state_ptr;
    *curr_list = state_ptr->outputs;
    *curr_node = state_ptr->outputs->head;
    return;
}
// Count the number of nodes connected to the tmp_node passed in;
// returns the number of nodes.
int count_nodes(node_t* tmp_node){
    // at least one node is initialized already
    int count_node=1;
    while(tmp_node->next != NULL){
        count_node++;
        tmp_node = tmp_node->next;
    }
    return count_node;
}
// Determine which strings to generate based on the input in STAGE 1&2,
// and generate text using helper functions.
void generate_text(state_t* curr_state, list_t* curr_list, 
node_t* curr_node, int pos, int printed){
    // variable initial is used to check whether the loop is initial iteration
    int i, count_node, initial=1;
    node_t* tmp_node;
    print_ellipses(CHLIMIT-printed);
    // loop until leaf state is met
    while (curr_node->str != NULL){
        tmp_node = curr_node;
        count_node = count_nodes(tmp_node);
        if (printed > CHLIMIT){
            break;
        // if there are multiple possible outputs
        } else if (count_node != 1){
            node_t* tmp2_node = tmp_node->next;
            for (i=1; i < count_node; i++){
                // tmp_node keeps the most appropriate node
                // based on the criteria
                tmp_node = cmp_state(tmp_node, tmp2_node);
                if (tmp2_node->next != NULL){
                    tmp2_node = tmp2_node->next;
                }
            }
        } 
        printed += print_str(tmp_node->str, pos, initial);
        curr_set(tmp_node->state, &curr_state, &curr_list, &curr_node);
        initial=RSTZERO;
    }
    printf("%c", NEWLNC);
    return;
}
// Print the outputs based on the inputs in STAGE 1&2,
// if initial, print from a character 
// after the input character was found in the string;
// returns the number of characters printed.
int print_str(char* string, int pos, int initial){
    int printed=0;
    if (initial){
        while (string[pos]){
            printf("%c", string[pos]);
            printed++;
            pos++;
        }
    } else {
        int str_len = strlen(string);
        printf("%s", string);
        // add the length of string to variable printed
        printed+= str_len-1;
    }
    return printed;
}
// Determine how many ellipses can be printed based on the space available,
// and prints ellipse(s) accordingly.
void print_ellipses(int space){
    int i;
    if (space >= NELLIP){
        space = NELLIP;
    } 
    for (i=0; i < space; i++){
        printf("%c", PERIDA);
    }
    return;
}
// Print the last character, before NEWLNC is encountered due to illegal inputs;
// returns RSTZERO to variable printed to reset the count of characters printed.
int print_wo_newln(int input_ascii, int nellipse){
    printf("%c", input_ascii);
    // the illegal character is printed, 
    // decrement the number of ellipse that can be printed by 1
    print_ellipses(nellipse-1);
    printf("%c", NEWLNC);
    while((input_ascii=mygetchar()) != NEWLNA);
    return RSTZERO;
}
// Compare the frequency of states and 
// returns the node that connects to state with greater frequency; 
// if frequency the same, returns the node with ascii-betically greater string.
node_t* cmp_state(node_t* tmp_node, node_t* tmp2_node){
    if ((tmp_node->state->freq) == (tmp2_node->state->freq)){
        char* tmp_str = tmp_node->str;
        char* tmp2_str = tmp2_node->str;
        int result = strcmp(tmp_str, tmp2_str);
        // if tmp_ch is greater
        if (result > 0){
            return tmp_node;
        } else {
            return tmp2_node;
        }
    // if freq is not equal
    } else if ((tmp_node->state->freq) > (tmp2_node->state->freq)){
        return tmp_node;
    } else {
        return tmp2_node;
    }
}
// Organize the nodes emerging from curr_list 
// for strings associated in ascii-betically decreasing order; 
// update the curr_list point to head and tail of the sorted nodes.
void sort_node(list_t** curr_list){
    // pointer to keep the largest value
    node_t* head_node = (*curr_list)->head;
    // pointer to keep the nodes up to where sorted
    node_t* sorted = (*curr_list)->head;
    // poitner to traverse the node
    node_t* tmp_node = sorted->next; 
    char* sorted_str, * tmp_str;
    int result;
    while (sorted->next != NULL){
        sorted_str = sorted->str;
        tmp_str = tmp_node->str;
        result = strcmp(sorted_str, tmp_str);
        // if the sorted_str is larger than tmp_str, 
        // it is sorted
        if (result >= 0){
            sorted = tmp_node;
            if (tmp_node->next != NULL){
                tmp_node = tmp_node->next;
            }
        // if not sorted
        } else if (result < 0){
            if (tmp_node->next != NULL){
                sorted->next = tmp_node->next;
            } else if (tmp_node->next == NULL){
                sorted->next = NULL;
            }
            node_t* tmp2_node = head_node;
            // a variable as a guard condition of following while loop
            int status=1;
            while (status){
                char* head_str = head_node->str;
                result = strcmp(tmp_str, head_str);
                if (result >= 0){
                    tmp_node->next = head_node;
                    head_node = tmp_node;
                    break;
                }
                if (tmp2_node->next != NULL){
                    node_t* tmp3_node = tmp2_node->next;
                    char* tmp3_str = tmp3_node->str;
                    result = strcmp(tmp_str, tmp3_str);
                    if (result >= 0){
                        tmp_node->next = tmp3_node;
                        tmp2_node->next = tmp_node;
                        break;
                    } else {
                        tmp2_node = tmp3_node;
                    }
                }      
                
            }

        }
    }
    // update curr_list head and tail
    (*curr_list)->head = head_node;
    (*curr_list)->tail = sorted;
    return;
}
// Create an array of pointers to states, following depth first order (DFO);
// returns a pointer to the array.
state_t** depth_first_search(automaton_t* automaton){
    int i, count_node, curr_size=0, capacity=0;
    state_t** dfo;
    capacity++;
    dfo = (state_t**)malloc(capacity * sizeof(*dfo));
    assert(dfo != NULL);
    // create a stack to put states into DFO 
    mystack_t* stack;
    stack = (mystack_t*)malloc(sizeof(stack));
    assert(stack != NULL);
    stack->top = NULL;
    // pointers to traverse the automaton
    state_t* curr_state; list_t* curr_list; 
    node_t* curr_node, * tmp_node, * tmp2_node;
    curr_set(automaton->ini, &curr_state, &curr_list, &curr_node);
    // push the first state into stack
    pushtop_stack(stack, curr_state);
    while (stack->top != NULL){
        count_node = count_nodes(curr_node);
        dfo[curr_size] = curr_state;
        curr_size++;
        // if more than one nodes coming out from curr_state
        if (count_node != 1){
            // organize nodes in ascii-betically decreasing order
            sort_node(&curr_list);
            tmp_node = curr_list->tail;
            tmp2_node = curr_list->head;
            for (i=1; i < count_node; i++){
                pushtop_stack(stack, tmp2_node->state);
                tmp2_node = tmp2_node->next;
            }
            // first, explore the ascii-betically smallest node
            curr_set(tmp_node->state, &curr_state, &curr_list, &curr_node); 
        // all explored   
        } else if (curr_node->str == NULL){
            // take the state at the top of the stack
            curr_set(stack->top->data, &curr_state, &curr_list, &curr_node); 
            poptop_stack(&stack);  
        // only one node from the state
        } else {
            curr_set(curr_node->state, &curr_state, &curr_list, &curr_node);
        } 
        // expand the size of dfo array by double if necessary
        if (curr_size >= capacity){
            capacity *= 2;
            dfo = (state_t **)realloc(dfo, capacity * sizeof(*dfo));
        }
    }
    // free the stack
    free(stack);
    stack = NULL;
    // update the number of states in automaton
    automaton->nid = curr_size;
    return dfo;
}
// Add a stacknode to the top of the stack; 
// Code between lines 736-748 is adapted version of push function
// from: https://iq.opengenus.org/depth-first-search/ 
void pushtop_stack(mystack_t* stack, state_t* curr_state){
    stacknode_t* stacknode;
    stacknode = (stacknode_t*)malloc(sizeof(stacknode));
    assert(stacknode != NULL);
    // Push the curr_state into stack
    stacknode->data = curr_state;
    // Update the new top node of stack
    if (stack->top != NULL){
        stacknode->next = stack->top;
    }
    stack->top = stacknode;
    return;
}
// Remove a stacknode from the top of the stack; 
// code between lines 752-766 is adapted version of pop function
// from: https://iq.opengenus.org/depth-first-search/ 
void poptop_stack(mystack_t** stack){
    stacknode_t* tmp_stacknode;
    tmp_stacknode = (*stack)->top;
    // if there's only one element in stack
    if (tmp_stacknode->next == NULL){
        (*stack)->top = NULL;
    // if there is more than one element in stack, 
    // pass the new top and,  
    } else {
        (*stack)->top = tmp_stacknode->next;
    }  
    // free the old top node in stack
    free(tmp_stacknode);
    tmp_stacknode = NULL;
}
// Free the automaton; 
// code between lines 770-779 is adapted version of free_tree function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/treeops.c
void free_automaton(state_t* ini, automaton_t* automaton){
    if(ini == NULL){
        return;
    }
    recursive_free_set(ini);
    // free the automaton_t pointer
    free(automaton);
    automaton = NULL;
    return;
}
// Recursively free nodes, list, and states; 
// code between lines 784-816 is adapted version of 
// recursive_free_tree function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/treeops.c
void recursive_free_set(state_t* state){
    if (state == NULL){
        return;
    }
    // if state is not null, there must be at lease one node
    int i, count_node;
    node_t* tmp_node = state->outputs->head;
    list_t* curr_list = state->outputs;
    count_node = count_nodes(tmp_node);
    // traverse over the possible output nodes from the state
    for (i=0; i < count_node; i++){
        // the node have have next state, 
        // recursively call the function again to free the next state
        if (tmp_node->state != NULL){
            recursive_free_set(tmp_node->state);
        }
        node_t* tmp2_node = tmp_node;
        // if the node is connected to another output from the state,
        // pass the tmp_node to the next one 
        if (tmp_node->next != NULL){
            tmp_node = tmp_node->next;
        }
        // free the node
        free(tmp2_node);
        tmp2_node = NULL;
    }
    // free the current list and state
    free(curr_list);
    curr_list = NULL;
    free(state);
    state = NULL;
    return;
}
/* THE END -------------------------------------------------------------------*/