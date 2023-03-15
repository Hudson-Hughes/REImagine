// hierarchy.cpp
#include "hierarchy.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

Node::Node() : parent(-1), first_child(-1), next_sibling(-1), previous_sibling(-1), level(0) {}

void Hierarchy::removeNodeRecursive(int node_index) {
    int child_index = nodes[node_index].first_child;
    while (child_index != -1) {
        int next_sibling_index = nodes[child_index].next_sibling;
        removeNodeRecursive(child_index);
        child_index = next_sibling_index;
    }
}

void Hierarchy::updateNodeLevelRecursive(int node_index, int new_level) {
    nodes[node_index].level = new_level;
    int child_index = nodes[node_index].first_child;
    while (child_index != -1) {
        updateNodeLevelRecursive(child_index, new_level + 1);
        child_index = nodes[child_index].next_sibling;
    }
}

void Hierarchy::printHierarchyRecursive(int node_index, int indent) {
    std::cout << std::setw(indent) << ' ' << "Node ID: " << node_index << std::endl;
    std::cout << "Parent: " << nodes[node_index].parent << " Child: " << nodes[node_index].first_child << " Next Sibling: " << nodes[node_index].next_sibling << " Previous Child: " << nodes[node_index].previous_sibling << std::endl << std::endl;
    int child_index = nodes[node_index].first_child;
    while (child_index != -1) {
        printHierarchyRecursive(child_index, indent + 4);
        child_index = nodes[child_index].next_sibling;
    }
}

int Hierarchy::addNode() {
    int new_index;
    if (recycle_bin.empty()) {
        nodes.emplace_back();
        new_index = nodes.size() - 1;
    } else {
        new_index = recycle_bin.top();
        recycle_bin.pop();
        nodes[new_index] = Node();
    }
    top_level_nodes.insert(new_index);
    return new_index;
}

void Hierarchy::addChild(int parent_index, int child_index) {
    if (parent_index < 0 || parent_index >= nodes.size() || child_index < 0 || child_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    Node& parent = nodes[parent_index];
    Node& child = nodes[child_index];

    child.parent = parent_index;
    child.level = parent.level + 1;
    child.next_sibling = parent.first_child;
    if (parent.first_child != -1) {
        nodes[parent.first_child].previous_sibling = child_index;
    }
    parent.first_child = child_index;

    top_level_nodes.erase(child_index);
}

int Hierarchy::addNodeAsChild(int parent_index) {
    int child_index = addNode();
    addChild(parent_index, child_index);
    return child_index;
}

bool Hierarchy::isDescendant(const std::vector<Node>& nodes, int ancestor, int descendant) {
    int current_parent = nodes[descendant].parent;
    while (current_parent != -1) {
        if (current_parent == ancestor) {
            return true;
        }
        current_parent = nodes[current_parent].parent;
    }
    return false;
}

void Hierarchy::moveToParent(int node_index, int new_parent_index) {
    if (node_index < 0 || node_index >= nodes.size() || new_parent_index < 0 || new_parent_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    Node& node = nodes[node_index];
    Node& new_parent = nodes[new_parent_index];

    if (isDescendant(nodes, node_index, new_parent_index)) {
        
        throw std::out_of_range("Attempting to swap ancestor/descendant relationship not supported yet");
        // New parent is a descendant of the node to be moved
        int old_parent_index = node.parent;
        Node& old_parent = nodes[old_parent_index];

        // Detach the new_parent from its current position
        if (new_parent.previous_sibling != -1) {
            nodes[new_parent.previous_sibling].next_sibling = new_parent.next_sibling;
        }
        if (new_parent.next_sibling != -1) {
            nodes[new_parent.next_sibling].previous_sibling = new_parent.previous_sibling;
        }
        if (new_parent.parent != -1) {
            Node& grandparent = nodes[new_parent.parent];
            if (grandparent.first_child == new_parent_index) {
                grandparent.first_child = new_parent.next_sibling;
            }
        }

        // Attach the node as the new_parent's parent
        if (node.first_child != -1) {
            Node& node_first_child = nodes[node.first_child];
            node_first_child.previous_sibling = new_parent_index;
        }
        new_parent.next_sibling = node.first_child;
        node.first_child = new_parent_index;
        new_parent.parent = node_index;
        new_parent.previous_sibling = -1;

        // Update the levels of the nodes
        updateNodeLevelRecursive(node_index, new_parent.level - 1);
        updateNodeLevelRecursive(new_parent_index, node.level + 1);
    } else {
        // Regular move to a new parent
        if (node.parent != -1) {
            Node& old_parent = nodes[node.parent];
            if (old_parent.first_child == node_index) {
                old_parent.first_child = node.next_sibling;
            }
        } else {
            // Remove the node from topLevelNodes if it was a top-level node
            top_level_nodes.erase(node_index);
        }
        if (node.previous_sibling != -1) {
            nodes[node.previous_sibling].next_sibling = node.next_sibling;
        }
        if (node.next_sibling != -1) {
            nodes[node.next_sibling].previous_sibling = node.previous_sibling;
        }

        // Attach the node as the new parent's new first child
        if (new_parent.first_child != -1) {
            Node& new_parent_first_child = nodes[new_parent.first_child];
            new_parent_first_child.previous_sibling = node_index;
        }
        node.next_sibling = new_parent.first_child;
        new_parent.first_child = node_index;
        node.parent = new_parent_index;
        node.previous_sibling = -1;

        // Update the levels of the nodes
        updateNodeLevelRecursive(node_index, nodes[new_parent_index].level + 1);
    }
}

void Hierarchy::removeNode(int node_index) {
    if (node_index < 0 || node_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    removeNodeRecursive(node_index);

    Node& node = nodes[node_index];
    if (node.parent != -1) {
        Node& parent = nodes[node.parent];
        if (parent.first_child == node_index) {
            parent.first_child = node.next_sibling;
        }
    }
    if (node.previous_sibling != -1) {
        nodes[node.previous_sibling].next_sibling = node.next_sibling;
    }
    if (node.next_sibling != -1) {
        nodes[node.next_sibling].previous_sibling = node.previous_sibling;
    }

    node.parent = -2; // Mark as deleted
    recycle_bin.push(node_index);
    top_level_nodes.erase(node_index);
}

int Hierarchy::getNodeLevel(int node_index) {
    if (node_index < 0 || node_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    return nodes[node_index].level;
}

int Hierarchy::getParent(int node_index) {
    if (node_index < 0 || node_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    return nodes[node_index].parent;
}

std::vector<int> Hierarchy::getChildren(int node_index) {
    if (node_index < 0 || node_index >= nodes.size()) {
        throw std::out_of_range("Invalid node index");
    }
    std::vector<int> children;
    int child_index = nodes[node_index].first_child;
    while (child_index != -1) {
        children.push_back(child_index);
        child_index = nodes[child_index].next_sibling;
    }
    return children;
}

std::vector<int> Hierarchy::getTopLevelNodes() {
    return std::vector<int>(top_level_nodes.begin(), top_level_nodes.end());
}

void Hierarchy::printHierarchy() {
    for (int node_index : top_level_nodes) {
        printHierarchyRecursive(node_index, 0);
    }
}

void Hierarchy::orphan(int node_id) {
    if (node_id < 0 || static_cast<size_t>(node_id) >= nodes.size()) {
        std::cerr << "Invalid node ID.\n";
        return;
    }

    Node& node = nodes[node_id];
    int parent_id = node.parent;

    if (parent_id == -1) {
        std::cerr << "Node is already an orphan.\n";
        return;
    }

    Node& parent = nodes[parent_id];

    if (parent.first_child == node_id) {
        parent.first_child = node.next_sibling;
    }

    if (node.previous_sibling != -1) {
        nodes[node.previous_sibling].next_sibling = node.next_sibling;
    }

    if (node.next_sibling != -1) {
        nodes[node.next_sibling].previous_sibling = node.previous_sibling;
    }

    node.parent = -1;
    node.previous_sibling = -1;
    node.next_sibling = -1;

    top_level_nodes.insert(node_id);
}

void userInterfaceLoop(Hierarchy& hierarchy) {
    std::string action_log_file = "action_log.txt";
    int option = 0;
    int node_id, parent_id;

    while (true) {
        hierarchy.printHierarchy();
        std::cout << "Choose an option:\n";
        std::cout << "1. Add node\n2. Add node as a child of parent\n3. Move node to be a child of parent\n";
        std::cout << "4. Remove node\n5. Get node level\n6. Get parent\n7. Get children\n8. Replay actions from file\n";
        std::cout << "9. Exit\n";
        std::cin >> option;

        std::ofstream action_log;
        action_log.open(action_log_file, std::ios::app);

        switch (option) {
            case 1:
                node_id = hierarchy.addNode();
                std::cout << "Added node with ID: " << node_id << std::endl;
                action_log << "1 " << node_id << std::endl;
                break;
            case 2:
                std::cout << "Enter parent node ID: ";
                std::cin >> parent_id;
                node_id = hierarchy.addNodeAsChild(parent_id);
                std::cout << "Added node with ID: " << node_id << " as a child of node " << parent_id << std::endl;
                action_log << "2 " << parent_id << " " << node_id << std::endl;
                break;
            case 3:
                std::cout << "Enter node ID to move: ";
                std::cin >> node_id;
                std::cout << "Enter new parent node ID: ";
                std::cin >> parent_id;
                hierarchy.moveToParent(node_id, parent_id);
                action_log << "3 " << node_id << " " << parent_id << std::endl;
                break;
            case 4:
                std::cout << "Enter node ID to remove: ";
                std::cin >> node_id;
                hierarchy.removeNode(node_id);
                action_log << "4 " << node_id << std::endl;
                break;
            case 5:
                std::cout << "Enter node ID to get its level: ";
                std::cin >> node_id;
                std::cout << "Node " << node_id << " is at level " << hierarchy.getNodeLevel(node_id) << std::endl;
                break;
            case 6:
                std::cout << "Enter node ID to get its parent: ";
                std::cin >> node_id;
                parent_id = hierarchy.getParent(node_id);
                if (parent_id != -1) {
                    std::cout << "Parent of node " << node_id << " is node " << parent_id << std::endl;
                } else {
                    std::cout << "Node " << node_id << " does not have a parent." << std::endl;
                }
                break;
            case 7:
                std::cout << "Enter node ID to get its children: ";
                std::cin >> node_id;
                std::cout << "Children of node " << node_id << ": ";
                for (int child_id : hierarchy.getChildren(node_id)) {
                    std::cout << child_id << " ";
                }
                std::cout << std::endl;
                break;
            case 8:
                {
                    std::ifstream infile(action_log_file);
                    std::string line;
                    while(std::getline(infile, line)) {
                        std::istringstream iss(line);
                        int action;
                        iss >> action;

                        switch (action) {
                            case 1:
                                iss >> node_id;
                                hierarchy.addNode();
                                break;
                            case 2:
                                iss >> parent_id >> node_id;
                                hierarchy.addNodeAsChild(parent_id);
                                break;
                            case 3:
                                iss >> node_id >> parent_id;
                                hierarchy.moveToParent(node_id, parent_id);
                                break;
                            case 4:
                                iss >> node_id;
                                hierarchy.removeNode(node_id);
                                break;
                        }
                    }
                    infile.close();
                }
                break;
            case 9:
                action_log.close();
                return;
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }

        action_log.close();
    }
}