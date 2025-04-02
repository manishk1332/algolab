#include <iostream>
using namespace std;

// Definition of a BST node.
struct Node {
    int key;
    Node* left;
    Node* right;
};

// Helper function to create a new BST node.
Node* newNode(int key) {
    Node* node = new Node;
    node->key = key;
    node->left = node->right = nullptr;
    return node;
}

// Insert a key into the BST and return the (possibly new) root.
Node* insert(Node* root, int key) {
    if (root == nullptr)
        return newNode(key);
    
    if (key < root->key)
        root->left = insert(root->left, key);
    else if (key > root->key)
        root->right = insert(root->right, key);
    
    // Duplicate keys are not inserted.
    return root;
}

// Search for a key in the BST; returns a pointer to the node if found, else nullptr.
Node* search(Node* root, int key) {
    if (root == nullptr || root->key == key)
        return root;
    
    return (key < root->key) ? search(root->left, key)
                             : search(root->right, key);
}

// Find the node with the minimum key in the BST.
Node* findMin(Node* root) {
    if (root == nullptr)
        return nullptr;
    while (root->left != nullptr)
        root = root->left;
    return root;
}

// Find the node with the maximum key in the BST.
Node* findMax(Node* root) {
    if (root == nullptr)
        return nullptr;
    while (root->right != nullptr)
        root = root->right;
    return root;
}

// Find the in-order successor of node x in the BST.
// The successor is the smallest node greater than x.
// If x has a right subtree, the successor is the minimum of that subtree.
// Otherwise, we search from the root for the deepest ancestor for which x is in the left subtree.
Node* findSuccessor(Node* root, Node* x) {
    if (x == nullptr)
        return nullptr;
    
    if (x->right != nullptr)
        return findMin(x->right);
    
    Node* succ = nullptr;
    while (root != nullptr) {
        if (x->key < root->key) {
            succ = root;
            root = root->left;
        } else if (x->key > root->key) {
            root = root->right;
        } else {
            break;
        }
    }
    return succ;
}

// Find the in-order predecessor of node x in the BST.
// The predecessor is the largest node smaller than x.
// If x has a left subtree, the predecessor is the maximum of that subtree.
// Otherwise, we search from the root for the deepest ancestor for which x is in the right subtree.
Node* findPredecessor(Node* root, Node* x) {
    if (x == nullptr)
        return nullptr;
    
    if (x->left != nullptr)
        return findMax(x->left);
    
    Node* pred = nullptr;
    while (root != nullptr) {
        if (x->key > root->key) {
            pred = root;
            root = root->right;
        } else if (x->key < root->key) {
            root = root->left;
        } else {
            break;
        }
    }
    return pred;
}

// Delete a node with the given key from the BST and return the new root.
Node* deleteNode(Node* root, int key) {
    if (root == nullptr)
        return root;
    
    // Recur down the tree to find the node to delete.
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // Node with only one child or no child.
        if (root->left == nullptr) {
            Node* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            Node* temp = root->left;
            delete root;
            return temp;
        }
        // Node with two children: Get the inorder successor (smallest in the right subtree).
        Node* temp = findMin(root->right);
        // Copy the inorder successor's key to this node.
        root->key = temp->key;
        // Delete the inorder successor.
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}

// Inorder traversal: Left, Root, Right.
void inorder(Node* root) {
    if (root != nullptr) {
        inorder(root->left);
        cout << root->key << " ";
        inorder(root->right);
    }
}

int main() {
    Node* root = nullptr;
    
    // Insert nodes into the BST.
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 70);
    root = insert(root, 60);
    root = insert(root, 80);
    
    cout << "Inorder traversal of the BST: ";
    inorder(root);
    cout << "\n";
    
    // Search for a key.
    int keyToSearch = 40;
    Node* found = search(root, keyToSearch);
    cout << "Search for " << keyToSearch << ": " << (found ? "Found" : "Not Found") << "\n";
    
    // Find the minimum and maximum keys.
    Node* minNode = findMin(root);
    Node* maxNode = findMax(root);
    if (minNode)
        cout << "Minimum key: " << minNode->key << "\n";
    if (maxNode)
        cout << "Maximum key: " << maxNode->key << "\n";
    
    // Find the successor and predecessor of a given node.
    Node* node50 = search(root, 50);
    Node* succ = findSuccessor(root, node50);
    Node* pred = findPredecessor(root, node50);
    if (succ)
        cout << "Successor of " << node50->key << ": " << succ->key << "\n";
    else
        cout << "No successor for " << node50->key << "\n";
    
    if (pred)
        cout << "Predecessor of " << node50->key << ": " << pred->key << "\n";
    else
        cout << "No predecessor for " << node50->key << "\n";
    
    // Delete nodes and show the resulting BST after each deletion.
    root = deleteNode(root, 20);
    cout << "Inorder traversal after deleting 20: ";
    inorder(root);
    cout << "\n";
    
    root = deleteNode(root, 30);
    cout << "Inorder traversal after deleting 30: ";
    inorder(root);
    cout << "\n";
    
    root = deleteNode(root, 50);
    cout << "Inorder traversal after deleting 50: ";
    inorder(root);
    cout << "\n";
    
    return 0;
}