#include <iostream>
using namespace std;

// Node structure representing a single element in the linked list.
struct Node {
    int data;
    Node* next;
};

// LinkedList class encapsulates common operations on a singly linked list.
class LinkedList {
private:
    Node* head;  // Pointer to the head (first node) of the list.
public:
    // Constructor initializes an empty list.
    LinkedList() : head(nullptr) {}

    // Destructor cleans up the list.
    ~LinkedList() {
        clear();
    }

    // Insert a new node with the given value at the beginning of the list.
    void insertAtHead(int value) {
        Node* new_node = new Node;
        new_node->data = value;
        new_node->next = head;
        head = new_node;
    }

    // Insert a new node with the given value at the end of the list.
    void insertAtTail(int value) {
        Node* new_node = new Node;
        new_node->data = value;
        new_node->next = nullptr;
        if (head == nullptr) {
            head = new_node;
            return;
        }
        Node* current = head;
        while (current->next != nullptr)
            current = current->next;
        current->next = new_node;
    }

    // Insert a new node with the given value at the specified position (0-indexed).
    // If the position is beyond the current length, the value is inserted at the tail.
    void insertAtPosition(int value, int position) {
        if (position <= 0) {
            insertAtHead(value);
            return;
        }
        Node* new_node = new Node;
        new_node->data = value;
        new_node->next = nullptr;

        Node* current = head;
        for (int i = 0; current != nullptr && i < position - 1; i++) {
            current = current->next;
        }
        if (current == nullptr) {
            // Position is out of range, so insert at tail.
            insertAtTail(value);
        } else {
            new_node->next = current->next;
            current->next = new_node;
        }
    }

    // Delete the first node that contains the specified value.
    void deleteValue(int value) {
        if (head == nullptr)
            return;

        // If the head node holds the value.
        if (head->data == value) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return;
        }

        Node* current = head;
        while (current->next != nullptr && current->next->data != value) {
            current = current->next;
        }
        if (current->next == nullptr) {
            cout << "Value " << value << " not found in the list." << endl;
            return;
        }
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
    }

    // Delete the head node.
    void deleteAtHead() {
        if (head == nullptr)
            return;
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    // Delete the tail node.
    void deleteAtTail() {
        if (head == nullptr)
            return;
        if (head->next == nullptr) {
            delete head;
            head = nullptr;
            return;
        }
        Node* current = head;
        while (current->next->next != nullptr)
            current = current->next;
        delete current->next;
        current->next = nullptr;
    }

    // Search for a node with the given value.
    // Returns a pointer to the node if found, otherwise returns nullptr.
    Node* search(int value) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value)
                return current;
            current = current->next;
        }
        return nullptr;
    }

    // Reverse the linked list.
    void reverse() {
        Node* prev = nullptr;
        Node* current = head;
        Node* next = nullptr;
        while (current != nullptr) {
            next = current->next; // store next node
            current->next = prev; // reverse the link
            prev = current;       // move prev forward
            current = next;       // move current forward
        }
        head = prev;
    }

    // Display the contents of the list.
    void display() {
        Node* current = head;
        while (current != nullptr) {
            cout << current->data;
            if (current->next != nullptr)
                cout << " -> ";
            current = current->next;
        }
        cout << endl;
    }

    // Clear the list by deleting all nodes.
    void clear() {
        while (head != nullptr) {
            deleteAtHead();
        }
    }
};

int main() {
    LinkedList list;

    // Insertion operations.
    list.insertAtHead(10);              // List: 10
    list.insertAtTail(20);              // List: 10 -> 20
    list.insertAtTail(30);              // List: 10 -> 20 -> 30
    list.insertAtPosition(15, 1);       // List: 10 -> 15 -> 20 -> 30
    cout << "List after insertions: ";
    list.display();

    // Searching for an element.
    int searchVal = 20;
    Node* found = list.search(searchVal);
    if (found)
        cout << "Value " << searchVal << " found in the list." << endl;
    else
        cout << "Value " << searchVal << " not found in the list." << endl;

    // Deletion operations.
    list.deleteValue(15);               // List becomes: 10 -> 20 -> 30
    cout << "List after deleting 15: ";
    list.display();

    list.deleteAtHead();                // List becomes: 20 -> 30
    cout << "List after deleting head: ";
    list.display();

    list.deleteAtTail();                // List becomes: 20
    cout << "List after deleting tail: ";
    list.display();

    // Insert additional elements and reverse the list.
    list.insertAtTail(40);              // List: 20 -> 40
    list.insertAtTail(50);              // List: 20 -> 40 -> 50
    cout << "List before reverse: ";
    list.display();
    list.reverse();
    cout << "List after reverse: ";
    list.display();

    return 0;
}