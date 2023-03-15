// tree.h
#ifndef TREE_H
#define TREE_H

#include <vector>
#include <stack>
#include <set>
#include <stdexcept>

class Node {
public:
    int parent;
    int first_child;
    int next_sibling;
    int previous_sibling;
    int level;

    Node();
};

class Hierarchy {
private:
    std::vector<Node> nodes;
    std::stack<int> recycle_bin;
    std::set<int> top_level_nodes;

    void removeNodeRecursive(int node_index);
    void updateNodeLevelRecursive(int node_index, int new_level);
    void printHierarchyRecursive(int node_index, int indent);
    bool isDescendant(const std::vector<Node>& nodes, int ancestor, int descendant);

public:
    int addNode();
    void addChild(int parent_index, int child_index);
    int addNodeAsChild(int parent_index);
    void moveToParent(int node_index, int new_parent_index);
    void removeNode(int node_index);
    int getNodeLevel(int node_index);
    int getParent(int node_index);
    std::vector<int> getChildren(int node_index);
    std::vector<int> getTopLevelNodes();
    void printHierarchy();
    void orphan(int node_id);
};

void userInterfaceLoop(Hierarchy& h);

#endif // TREE_H