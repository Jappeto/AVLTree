// BinaryTreeNode.hpp

#ifndef BinaryTreeNode_hpp
#define BinaryTreeNode_hpp

#include <iostream>

#include <memory>

typedef int ItemType;

class BinaryTreeNode {
    friend class AVLTree;

public:
    BinaryTreeNode(const ItemType item,
        const std::shared_ptr<BinaryTreeNode> leftNode = nullptr,
        const std::shared_ptr<BinaryTreeNode> rightNode = nullptr,
        const std::shared_ptr<BinaryTreeNode>& parentNode = nullptr);

    int height() const { return _height; }
    void setHeight(const int height) { _height = height; }
    ItemType item() const { return _item; }


    //     ~BinaryTreeNode() noexcept { std::cerr << "deallocate BinaryTreeNode " << _item << std::endl; }


private:
    ItemType _item;
    std::shared_ptr<BinaryTreeNode> _leftNode;
    std::shared_ptr<BinaryTreeNode> _rightNode;
    std::weak_ptr<BinaryTreeNode> _parentNode;
    int _height;
};

inline BinaryTreeNode::BinaryTreeNode(const ItemType item,
    const std::shared_ptr<BinaryTreeNode> leftNode,
    const std::shared_ptr<BinaryTreeNode> rightNode,
    const std::shared_ptr<BinaryTreeNode>& parentNode) {
    _item = item;
    _leftNode = leftNode;
    _rightNode = rightNode;
    _parentNode = parentNode;
    _height = 0;
}

inline int getHeight(const std::shared_ptr<BinaryTreeNode> node) {
    if (node == nullptr)
        return -1;
    else
        return node->height();
}

#endif /* BinaryTreeNode_hpp */