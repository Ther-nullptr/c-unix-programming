#include <stdio.h>
#include <stdlib.h>

struct my_tree_node {
    struct my_tree_node *left;
    struct my_tree_node *right;
    int data;
};

// initialize a tree node
#define INIT_MY_TREE_NODE(ptr, value) \
        (ptr)->left = NULL;           \
        (ptr)->right = NULL;          \
        (ptr)->data = value;          

// insert a new node into the binary search tree
void my_tree_insert(struct my_tree_node **root, struct my_tree_node *new_node) {
    if (*root == NULL) {
        *root = new_node;
    } else if (new_node->data < (*root)->data) {
        my_tree_insert(&((*root)->left), new_node);
    } else {
        my_tree_insert(&((*root)->right), new_node);
    }
}

// in order traversal (left -> root -> right)
void my_tree_inorder(struct my_tree_node *root) {
    if (root != NULL) {
        my_tree_inorder(root->left);
        printf("%d ", root->data);
        my_tree_inorder(root->right);
    }
}

// pre order traversal (root -> left -> right)
void my_tree_preorder(struct my_tree_node *root) {
    if (root != NULL) {
        printf("%d ", root->data);
        my_tree_preorder(root->left);
        my_tree_preorder(root->right);
    }
}

// post order traversal (left -> right -> root)
void my_tree_postorder(struct my_tree_node *root) {
    if (root != NULL) {
        my_tree_postorder(root->left);
        my_tree_postorder(root->right);
        printf("%d ", root->data);
    }
}

// level order traversal
void my_tree_levelorder(struct my_tree_node *root) {
    if (root == NULL) {
        return;
    }

    struct my_tree_node *queue[1000];
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        struct my_tree_node *node = queue[front++];
        printf("%d ", node->data);

        if (node->left != NULL) {
            queue[rear++] = node->left;
        }
        if (node->right != NULL) {
            queue[rear++] = node->right;
        }
    }
}

// create a new tree node
struct my_tree_node* my_tree_create_node(int data) {
    struct my_tree_node *new_node = (struct my_tree_node *)malloc(sizeof(struct my_tree_node));
    if (!new_node) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    INIT_MY_TREE_NODE(new_node, data);
    return new_node;
}

// clean up the tree
void my_tree_free(struct my_tree_node *root) {
    if (root != NULL) {
        my_tree_free(root->left);
        my_tree_free(root->right);
        free(root);
    }
}

int main() {
    struct my_tree_node *root = NULL;

    printf("input: (-1 for break)\n");

    // input values for the tree from command line
    while (1) {
        int value;
        scanf("%d", &value);
        if (value == -1) {
            break;
        }
        struct my_tree_node *new_node = my_tree_create_node(value);
        my_tree_insert(&root, new_node);
    }

    // level order traversal
    printf("inorder: \n");
    my_tree_inorder(root);
    printf("\n");

    // pre order traversal
    my_tree_free(root);

    return 0;
}
