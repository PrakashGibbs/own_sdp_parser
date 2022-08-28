struct linked_list
{
    int number;
    struct linked_list *next;
};

typedef struct linked_list node;

void print_list( node* head);
void insert_at_last(int value, node* head);
void insert_at_first(int value, node* head);
void insert_after(int key, int value, node* head);
void delete_item(int value, node* head);
void search_item(int value, node* head);
void destroy_list(node* head);


