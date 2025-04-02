#include <iostream>
using namespace std;

// Node structure for AVL tree
struct Node {
    int key;
    Node* left;
    Node* right;
    int height;
};

// Utility function to get the height of the tree
int height(Node* N) {
    return (N == nullptr) ? 0 : N->height;
}

// Utility function to get maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Helper function to create a new AVL tree node
Node* newNode(int key) {
    Node* node = new Node;
    node->key = key;
    node->left = node->right = nullptr;
    node->height = 1;  // new node is initially added at leaf
    return node;
}

// Right rotate subtree rooted with y
Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x
Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Get balance factor of node N
int getBalance(Node* N) {
    return (N == nullptr) ? 0 : height(N->left) - height(N->right);
}

// Recursive function to insert a key into the subtree rooted
// with node and returns the new root of the subtree.
Node* insert(Node* node, int key) {
    // 1. Perform the normal BST insertion
    if (node == nullptr)
        return newNode(key);
    
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // Equal keys are not allowed in BST
        return node;

    // 2. Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // 3. Get the balance factor of this ancestor node to check whether
    // this node became unbalanced
    int balance = getBalance(node);

    // If node becomes unbalanced, then there are 4 cases:

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // Return the unchanged node pointer
    return node;
}

// Utility function to find the node with the minimum key value
// found in that tree.
Node* minValueNode(Node* node) {
    Node* current = node;
    while (current && current->left != nullptr)
        current = current->left;
    return current;
}

// Recursive function to delete a node with given key from subtree with
// given root. It returns root of the modified subtree.
Node* deleteNode(Node* root, int key) {
    // STEP 1: Perform standard BST delete
    if (root == nullptr)
        return root;
    
    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    // If the key to be deleted is greater than the root's key,
    // then it lies in right subtree
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    // if key is same as root's key, then this is the node to be deleted
    else {
        // node with only one child or no child
        if ((root->left == nullptr) || (root->right == nullptr)) {
            Node* temp = root->left ? root->left : root->right;
            
            // No child case
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else { // One child case: Copy the contents of the non-empty child
                *root = *temp;
            }
            delete temp;
        } else {
            // node with two children: Get the inorder successor (smallest in the right subtree)
            Node* temp = minValueNode(root->right);
            // Copy the inorder successor's key to this node
            root->key = temp->key;
            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // If the tree had only one node then return
    if (root == nullptr)
        return root;

    // STEP 2: Update the height of the current node
    root->height = 1 + max(height(root->left), height(root->right));

    // STEP 3: Get the balance factor of this node to check whether
    // this node became unbalanced
    int balance = getBalance(root);

    // If this node becomes unbalanced, then there are 4 cases:

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Recursive function to search a key in AVL tree.
bool search(Node* root, int key) {
    if (root == nullptr)
        return false;
    if (root->key == key)
        return true;
    return (key < root->key) ? search(root->left, key) : search(root->right, key);
}

// Utility function to print preorder traversal of the tree.
void preOrder(Node* root) {
    if (root != nullptr) {
        cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

int main() {
    Node* root = nullptr;
    
    // Insert nodes
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30);
    root = insert(root, 40);
    root = insert(root, 50);
    root = insert(root, 25);

    cout << "Preorder traversal of the constructed AVL tree is:" << endl;
    preOrder(root);
    cout << endl;

    // Search for a key
    int keyToSearch = 25;
    cout << "Searching for " << keyToSearch << ": " 
         << (search(root, keyToSearch) ? "Found" : "Not Found") << endl;

    // Delete a node and print preorder traversal after deletion
    root = deleteNode(root, 40);
    cout << "Preorder traversal after deletion of 40:" << endl;
    preOrder(root);
    cout << endl;

    return 0;
}