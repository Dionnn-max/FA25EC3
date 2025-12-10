
#ifndef FA25EC3_TREE_H
#define FA25EC3_TREE_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*
   TEMPLATE TREE CLASS
   Multiple parents may point to the same child.
   Students must manage storage, pointers, and dynamic allocation.

   Requirements:
   1. Templated Tree class.
   2. Nodes stored dynamically.
   3. Each node has:
      - id
      - data
      - vector of children pointers
   4. Students must prevent memory leaks.
   5. Students must create createRoot, addNode, findNode, print functions, etc.

   DO NOT IMPLEMENT ANYTHING HERE.
   Only placeholders and TODO comments.
*/

// Node class for tree
template <typename T>
class Node {
public:
    string id;
    T data;
    vector<Node<T>*> children;  // List of child nodes
    Node(string nodeId,T nodeData) {
        id = nodeId;
        data = nodeData;
    }
};

template <typename T>
class Tree{
private:
    Node<T>*root;  // First node of the tree
    Node<T>* searchNode(Node<T>* start, string targetId, vector<Node<T>*>&visited){
        if (start == nullptr) return nullptr;
        for (Node<T>* n : visited){
            if (n == start)
                return nullptr;
        }
        visited.push_back(start);
        // Check the current node
        if (start->id == targetId)
            return start;
        // Check all children
        for (Node<T>*child : start->children){
            Node<T>*found = searchNode(child,targetId,visited);
            if (found != nullptr)
                return found;
        }
        return nullptr;
    }
    void printNode(Node<T>* node,vector<Node<T>*>&visited){
        if (node == nullptr)
            return;
        // Skip if already printed
        for (Node<T>*n : visited){
            if (n == node)
                return;
        }
        visited.push_back(node);
        // Print this node
        cout << "Node " << node->id << ": " << node->data << endl;
        // Print children
        if (node->children.empty()){
            cout << "  Child -> (none)" << endl;
        }
        else{
            for (Node<T>*child : node->children){
                cout << "  Child -> " << child->id << endl;
            }
        }
        cout << endl;
        // Print all children
        for (Node<T>* child : node->children){
            printNode(child,visited);
        }
    }
    // Helper: Delete all nodes
    void deleteNodes(Node<T>*node, vector<Node<T>*>&visited){
        if (node == nullptr)
            return;
        // Skip if already deleted
        for (Node<T>* n : visited){
            if (n == node)
                return;
        }
        visited.push_back(node);
        for (Node<T>* child : node->children){
            deleteNodes(child,visited);
        }
        // Delete this node
        delete node;
    }

public:
    Tree() {
        root = nullptr;
    }

    // Create the first node
    void createRoot(string id, T value){
        if (root != nullptr){
            cout << "Warning: Tree already has a root!" << endl;
            return;
        }
        root = new Node<T>(id, value);
    }

    // Add a node to the tree
    void addNode(string parentId,string childId,T value){
        // Find parent node
        Node<T>* parent = findNode(parentId);
        if (parent == nullptr) {
            cout << "Error: Parent node " << parentId << " not found!" << endl;
            return;
        }

        // Check if child already exists
        Node<T>* existingChild = findNode(childId);
        if (existingChild != nullptr) {
            parent->children.push_back(existingChild);
        } else {
            // Create new child node
            Node<T>* newChild = new Node<T>(childId, value);
            parent->children.push_back(newChild);
        }
    }
    // Find a node by ID
    Node<T>* findNode(string id){
        if (root == nullptr) return nullptr;
        vector<Node<T>*> visited;
        return searchNode(root,id,visited);
    }
    // Print the whole tree
    void printAll(){
        if (root == nullptr){
            cout << "Tree is empty!" << endl;
            return;
        }
        cout << "\n===== Story Tree =====" << endl;
        vector<Node<T>*> visited;
        printNode(root,visited);
        cout << "======================" << endl;
    }
    // Play the game
    void playGame(){
        if (root == nullptr){
            cout << "No story to play!" << endl;
            return;
        }
        Node<T>* current = root;
        cout << "\n===== Begin Adventure =====\n" << endl;
        // Keep going until we reach an ending
        while (current != nullptr){
            // Show current story text
            cout << current->data << endl;
            // Check if this is an ending
            if (current->children.empty()){
                cout << "\n★ There are no further paths." << endl;
                cout << "★ Your journey ends here.\n" << endl;
                break;
            }
            cout << "\nChoose your next action:" << endl;
            for (int i = 0; i < current->children.size(); i++){
                cout << (i + 1) << ". Continue" << endl;
            }
            // Get user choice
            int choice;
            cout << "Selection: ";
            cin >> choice;

            // Validate input
            if (cin.fail()){
                cout << "Invalid input! Please enter a number." << endl;
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }

            if (choice < 1 || choice > current->children.size()){
                cout << "Invalid choice! Please try again." << endl;
                continue;
            }
            // Move to chosen path
            current = current->children[choice - 1];
            cout << endl;
        }

        cout << "===== Adventure Complete =====" << endl;
    }

    // clean up the memory
    ~Tree(){
        if (root == nullptr) return;
        vector<Node<T>*> visited;
        deleteNodes(root, visited);
        root = nullptr;
    }
};
#endif //FA25EC3_TREE_H