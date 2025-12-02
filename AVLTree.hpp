// AVLTree.hpp

#ifndef AVLTree_hpp
#define AVLTree_hpp

#include <vector>

#include "BinaryTreeNode.hpp"

class AVLTree {

public:
    AVLTree();

    // MARK: - methods for dynamic memory classes

    /// copy constructor
    AVLTree(const AVLTree& source);

    /// assignment operator
    AVLTree& operator=(const AVLTree& source);

    // MARK: - public methods

    /// returns number of items inserted into the tree
    size_t count() const { return _count; }

    /// removes all elements from the tree
    void clear();

    /// inserts item into binary search tree and maintains AVL balancing property
    /// - Parameter item: item to insert
    void insert(const ItemType& item);

    /// returns node containing item or nullptr if not in tree
    /// - Parameter item: item to search for
    std::shared_ptr<BinaryTreeNode> find(const ItemType& item) const;

    ///  returns node containing the minimum element; returns nullptr if the tree is empty
    std::shared_ptr<BinaryTreeNode> minimumNode() const;

    /// returns node containing the maximum element; returns nullptr if the tree is empty
    std::shared_ptr<BinaryTreeNode> maximumNode() const;

    /// returns the node containing the next smallest item in the tree than the item at the specified node; returns nullptr if node is nullptr or is the node with the minimum value in the tree
    /// - Parameter node: node whose item to use to find next smallest item
    std::shared_ptr<BinaryTreeNode> nextSmallestNode(std::shared_ptr<BinaryTreeNode> node) const;

    /// returns the node containing the next largest item in the tree than the item at the specified node; returns nullptr if node is nullptr or is the node with the maximum value in the tree
    /// - Parameter node: node whose item to use to find next largest item
    std::shared_ptr<BinaryTreeNode> nextLargestNode(std::shared_ptr<BinaryTreeNode> node) const;

    /// returns a vector containing the elements of the tree for an inorder traversal
    std::vector<ItemType> inorder() const;

    /// returns a vector containing the elements of the tree for a preorder traversal
    std::vector<ItemType> preorder() const;

    /// returns a vector containing the elements of the tree for a postorder traversal
    std::vector<ItemType> postorder() const;

private:
    /// returns a new shallow copy of a tree rooted at rootNode
    /// - Parameter rootNode: root of subtree to copy
    std::shared_ptr<BinaryTreeNode> _copyNodes(const std::shared_ptr<BinaryTreeNode>& rootNode) const;

    /// returns the node containing item or nullptr if not found in the subtree with specified root
    /// - Parameters:
    ///   - rootNode: root of subtree to search
    ///   - item: item to search for
    std::shared_ptr<BinaryTreeNode> _findHelp(const std::shared_ptr<BinaryTreeNode> rootNode, const ItemType& item) const;

    /// returns the node containing the minimum node in tree with specified root
    /// - Parameter rootNode: root of subtree to find the minimum in
    std::shared_ptr<BinaryTreeNode> _minimumNodeHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const;

    /// returns the node containing the maximum node in tree with specified root
    /// - Parameter rootNode: root of subtree to find the maximum in
    std::shared_ptr<BinaryTreeNode> _maximumNodeHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const;

    /// insert item in tree rooted at rootNode
    /// - Parameters:
    ///   - rootNode: rootNode of tree to insert in which is passed by reference since rotation may change it
    ///   - item: item to insert
    void _insertHelp(std::shared_ptr<BinaryTreeNode>& rootNode, const ItemType& item);

    /// inorder traversal helper
    /// - Parameter rootNode: root of subtree to run traversal on
    std::vector<ItemType> _inorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const;

    /// preorder traversal helper
    /// - Parameter rootNode: root of subtree to run traversal on
    std::vector<ItemType> _preorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const;

    /// postorder traversal helper
    /// - Parameter rootNode: root of subtree to run traversal on
    std::vector<ItemType> _postorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const;

    /// rotation helper
    /// - Parameter node: node to perform rotation at
    void _leftSingleRotate(std::shared_ptr<BinaryTreeNode>& node);

    /// rotation helper
    /// - Parameter node: node to perform rotation at
    void _rightSingleRotate(std::shared_ptr<BinaryTreeNode>& node);

    /// rotation helper
    /// - Parameter node: node to perform rotation at
    void _rightLeftRotate(std::shared_ptr<BinaryTreeNode>& node);

    /// rotation helper
    /// - Parameter node: node to perform rotation at
    void _leftRightRotate(std::shared_ptr<BinaryTreeNode>& node);

    /// pointer to root node of tree
    std::shared_ptr<BinaryTreeNode> _root;
    /// number of items in the tree
    size_t _count;
};

#endif /* AVLTree_hpp */