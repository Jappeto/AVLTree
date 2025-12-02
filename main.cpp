// main.cpp — extended coverage WITH explicit nullptr tests
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <exception>
#include <limits>   // INT_MIN / INT_MAX
#include <type_traits>
#include "AVLTree.hpp"

// ---------- tiny test harness ----------
#define EXPECT_TRUE(cond)  do { if (!(cond)) { \
    std::cerr << "[FAIL] " << __FUNCTION__ << ":" << __LINE__ \
              << "  EXPECT_TRUE(" #cond ") failed.\n"; failures++; } } while(0)

#define EXPECT_EQ(a,b)    do { auto _a=(a); auto _b=(b); \
    if (!((_a)==(_b))) { \
      std::cerr << "[FAIL] " << __FUNCTION__ << ":" << __LINE__ \
                << "  EXPECT_EQ(" #a ", " #b ") failed. Got [" << _a \
                << "] vs [" << _b << "].\n"; failures++; } } while(0)

template <typename T>
static void expect_vec_eq(const std::vector<T>& got,
    const std::vector<T>& want,
    int& failures,
    const char* func,
    int line,
    const char* label) {
    if (got != want) {
        std::cerr << "[FAIL] " << func << ":" << line << "  " << label
            << " mismatch.\n  Got:  ";
        for (const auto& x : got) std::cerr << x << " ";
        std::cerr << "\n  Want: ";
        for (const auto& x : want) std::cerr << x << " ";
        std::cerr << "\n";
        failures++;
    }
}
#define EXPECT_VEC_EQ(got, want, label) \
    expect_vec_eq(got, want, failures, __FUNCTION__, __LINE__, label)

static int failures = 0;

template <typename T>
static void print_vec(const std::string& label, const std::vector<T>& v) {
    std::cout << std::left << std::setw(18) << label << ": ";
    for (const auto& x : v) std::cout << x << " ";
    std::cout << "\n";
}

// pointer-identity check (shared_ptr address equality)
static void expect_same_node(const std::shared_ptr<BinaryTreeNode>& a,
    const std::shared_ptr<BinaryTreeNode>& b,
    const char* what) {
    if (a == b) {
        std::cout << "[PASS] " << what << "\n";
    }
    else {
        std::cerr << "[FAIL] " << what << " (node pointers differ)\n";
        failures++;
    }
}

// ---------------- tests ----------------
static void test_empty_tree_basics_and_null_ok() {
    std::cout << "\n== test_empty_tree_basics_and_null_ok ==\n";
    AVLTree t;

    EXPECT_EQ(t.count(), static_cast<size_t>(0));
    EXPECT_TRUE(t.inorder().empty());
    EXPECT_TRUE(t.preorder().empty());
    EXPECT_TRUE(t.postorder().empty());
    EXPECT_TRUE(t.minimumNode() == nullptr);
    EXPECT_TRUE(t.maximumNode() == nullptr);

    // Your impl: next* must safely accept nullptr and return nullptr.
    EXPECT_TRUE(t.nextSmallestNode(nullptr) == nullptr);
    EXPECT_TRUE(t.nextLargestNode(nullptr) == nullptr);

    // If ItemType is default constructible, find on empty should be safe and return nullptr.
    if (std::is_default_constructible<ItemType>::value) {
        ItemType dummy{};
        EXPECT_TRUE(t.find(dummy) == nullptr);
    }
}

static void test_singleton_boundaries_and_null_again() {
    std::cout << "\n== test_singleton_boundaries_and_null_again ==\n";
    AVLTree t;
    ItemType x = static_cast<ItemType>(42);
    t.insert(x);

    EXPECT_EQ(t.count(), static_cast<size_t>(1));
    EXPECT_VEC_EQ(t.inorder(), std::vector<ItemType>{x}, "inorder singleton");
    EXPECT_VEC_EQ(t.preorder(), std::vector<ItemType>{x}, "preorder singleton");
    EXPECT_VEC_EQ(t.postorder(), std::vector<ItemType>{x}, "postorder singleton");

    auto n = t.find(x);
    EXPECT_TRUE(n != nullptr);
    expect_same_node(n, t.minimumNode(), "singleton min is node");
    expect_same_node(n, t.maximumNode(), "singleton max is node");
    EXPECT_TRUE(t.nextSmallestNode(n) == nullptr);
    EXPECT_TRUE(t.nextLargestNode(n) == nullptr);

    // Re-assert null tolerance in non-empty context
    EXPECT_TRUE(t.nextSmallestNode(nullptr) == nullptr);
    EXPECT_TRUE(t.nextLargestNode(nullptr) == nullptr);
}

static void test_insert_and_traversals_basic() {
    std::cout << "\n== test_insert_and_traversals_basic ==\n";
    AVLTree t;
    std::vector<ItemType> data = { 10, 20, 5, 4, 8, 15, 30, 25, 40, 7, 9, 6 };
    for (const auto& x : data) t.insert(x);

    EXPECT_EQ(t.count(), data.size());

    auto in = t.inorder();
    auto want_in = data; std::sort(want_in.begin(), want_in.end());
    EXPECT_VEC_EQ(in, want_in, "inorder");

    auto pre = t.preorder();
    auto post = t.postorder();
    auto sorted_pre = pre, sorted_post = post;
    std::sort(sorted_pre.begin(), sorted_pre.end());
    std::sort(sorted_post.begin(), sorted_post.end());
    EXPECT_VEC_EQ(sorted_pre, want_in, "preorder elements");
    EXPECT_VEC_EQ(sorted_post, want_in, "postorder elements");

    print_vec("inorder", in);
    print_vec("preorder", pre);
    print_vec("postorder", post);
}

static void test_min_max_and_find() {
    std::cout << "\n== test_min_max_and_find ==\n";
    AVLTree t;
    std::vector<ItemType> data = { 50, 40, 30, 20, 10 };
    for (const auto& x : data) t.insert(x);

    auto min_node = t.minimumNode();
    auto max_node = t.maximumNode();
    EXPECT_TRUE(min_node != nullptr);
    EXPECT_TRUE(max_node != nullptr);

    auto sorted = data; std::sort(sorted.begin(), sorted.end());
    ItemType minv = sorted.front();
    ItemType maxv = sorted.back();

    auto min_via_find = t.find(minv);
    auto max_via_find = t.find(maxv);
    EXPECT_TRUE(min_via_find != nullptr);
    EXPECT_TRUE(max_via_find != nullptr);

    expect_same_node(min_node, min_via_find, "minimumNode == find(min)");
    expect_same_node(max_node, max_via_find, "maximumNode == find(max)");

    EXPECT_TRUE(t.find(30) != nullptr);
    EXPECT_TRUE(t.find(static_cast<ItemType>(999999)) == nullptr);
}

static void test_next_smallest_largest_boundaries_and_null() {
    std::cout << "\n== test_next_smallest_largest_boundaries_and_null ==\n";
    AVLTree t;
    std::vector<ItemType> data = { 10, 20, 30, 40, 50, 60 };
    for (const auto& x : data) t.insert(x);

    // null inputs again (non-empty tree)
    EXPECT_TRUE(t.nextSmallestNode(nullptr) == nullptr);
    EXPECT_TRUE(t.nextLargestNode(nullptr) == nullptr);

    // predecessor(min) -> nullptr
    ItemType minv = *std::min_element(data.begin(), data.end());
    auto minn = t.find(minv);
    EXPECT_TRUE(minn != nullptr);
    auto pred_min = t.nextSmallestNode(minn);
    EXPECT_TRUE(pred_min == nullptr);

    // successor(max) -> nullptr
    ItemType maxv = *std::max_element(data.begin(), data.end());
    auto maxn = t.find(maxv);
    EXPECT_TRUE(maxn != nullptr);
    auto succ_max = t.nextLargestNode(maxn);
    EXPECT_TRUE(succ_max == nullptr);

    // interior example
    auto node_mid = t.find(30);
    EXPECT_TRUE(node_mid != nullptr);
    auto pred_mid = t.nextSmallestNode(node_mid);
    auto succ_mid = t.nextLargestNode(node_mid);
    EXPECT_TRUE(pred_mid != nullptr);
    EXPECT_TRUE(succ_mid != nullptr);
    expect_same_node(pred_mid, t.find(20), "pred(30) == 20");
    expect_same_node(succ_mid, t.find(40), "succ(30) == 40");
}

// Explicit structural scenarios for successor/predecessor
static void test_predecessor_successor_structural() {
    std::cout << "\n== test_predecessor_successor_structural ==\n";
    AVLTree t;
    //            20
    //          /    \
    //        10      30
    //       / \     /  \
    //      5  15   25  40
    //           \        \
    //            17       45
    std::vector<ItemType> seq = { 20,10,30,5,15,25,40,17,45 };
    for (auto v : seq) t.insert(v);

    auto n15 = t.find(15);          // has right child (17)
    auto succ_15 = t.nextLargestNode(n15);  // via right subtree min
    expect_same_node(succ_15, t.find(17), "succ(15) == 17 (right-subtree min)");

    auto n17 = t.find(17);          // no right child, climb ancestors
    auto succ_17 = t.nextLargestNode(n17);
    expect_same_node(succ_17, t.find(20), "succ(17) == 20 (ancestor climb)");

    auto n30 = t.find(30);          // has left child (25)
    auto pred_30 = t.nextSmallestNode(n30); // via left subtree max
    expect_same_node(pred_30, t.find(25), "pred(30) == 25 (left-subtree max)");

    auto n25 = t.find(25);          // no left child, climb ancestors
    auto pred_25 = t.nextSmallestNode(n25);
    expect_same_node(pred_25, t.find(20), "pred(25) == 20 (ancestor climb)");
}

static void test_rotations_patterns() {
    std::cout << "\n== test_rotations_patterns ==\n";
    {
        AVLTree t;
        std::vector<ItemType> a = { 1,2,3,4,5,6,7 };
        for (auto v : a) t.insert(v);
        EXPECT_VEC_EQ(t.inorder(), a, "LL inorder");
        EXPECT_EQ(t.count(), a.size());
    }
    {
        AVLTree t;
        std::vector<ItemType> a = { 7,6,5,4,3,2,1 };
        for (auto v : a) t.insert(v);
        std::sort(a.begin(), a.end());
        EXPECT_VEC_EQ(t.inorder(), a, "RR inorder");
        EXPECT_EQ(t.count(), a.size());
    }
    {
        AVLTree t;
        std::vector<ItemType> a = { 30,10,20 };
        for (auto v : a) t.insert(v);
        std::vector<ItemType> want = { 10,20,30 };
        EXPECT_VEC_EQ(t.inorder(), want, "LR inorder");
        EXPECT_EQ(t.count(), a.size());
    }
    {
        AVLTree t;
        std::vector<ItemType> a = { 10,30,20 };
        for (auto v : a) t.insert(v);
        std::vector<ItemType> want = { 10,20,30 };
        EXPECT_VEC_EQ(t.inorder(), want, "RL inorder");
        EXPECT_EQ(t.count(), a.size());
    }
}

static void test_long_monotone_sequences() {
    std::cout << "\n== test_long_monotone_sequences ==\n";
    {
        AVLTree t;
        const int N = 200; // modest for classroom builds
        for (int i = 1; i <= N; ++i) t.insert(static_cast<ItemType>(i));
        auto in = t.inorder();
        std::vector<ItemType> want(N);
        for (int i = 0; i < N; ++i) want[i] = static_cast<ItemType>(i + 1);
        EXPECT_VEC_EQ(in, want, "ascending 1..N inorder");
        EXPECT_EQ(t.count(), static_cast<size_t>(N));
    }
    {
        AVLTree t;
        const int N = 200;
        for (int i = N; i >= 1; --i) t.insert(static_cast<ItemType>(i));
        auto in = t.inorder();
        std::vector<ItemType> want(N);
        for (int i = 0; i < N; ++i) want[i] = static_cast<ItemType>(i + 1);
        EXPECT_VEC_EQ(in, want, "descending N..1 inorder");
        EXPECT_EQ(t.count(), static_cast<size_t>(N));
    }
}

static void test_copy_ctor_and_assignment() {
    std::cout << "\n== test_copy_ctor_and_assignment ==\n";
    AVLTree t1;
    std::vector<ItemType> data = { 10, 5, 15, 2, 7, 12, 18 };
    for (auto v : data) t1.insert(v);

    AVLTree t2 = t1; // copy-ctor
    EXPECT_EQ(t2.count(), t1.count());
    EXPECT_VEC_EQ(t2.inorder(), t1.inorder(), "copy-ctor inorder");

    t1.clear(); // mutate source; t2 should remain intact
    EXPECT_EQ(t1.count(), static_cast<size_t>(0));
    EXPECT_EQ(t2.count(), static_cast<size_t>(data.size()));

    AVLTree t3;
    t3 = t2; // assignment
    EXPECT_VEC_EQ(t3.inorder(), t2.inorder(), "assign inorder");

    // self-assignment shouldn’t crash or corrupt memory
    t3 = t3;
    EXPECT_VEC_EQ(t3.inorder(), t2.inorder(), "self-assign inorder");
}

static void test_empty_copy_assign_selfassign() {
    std::cout << "\n== test_empty_copy_assign_selfassign ==\n";
    AVLTree empty;

    // copy-construct from empty
    AVLTree cpy = empty;
    EXPECT_EQ(cpy.count(), static_cast<size_t>(0));
    EXPECT_TRUE(cpy.inorder().empty());

    // assign from empty
    AVLTree t;
    t.insert(static_cast<ItemType>(1));
    t = empty;
    EXPECT_EQ(t.count(), static_cast<size_t>(0));
    EXPECT_TRUE(t.inorder().empty());

    // self-assign empty
    empty = empty;
    EXPECT_EQ(empty.count(), static_cast<size_t>(0));
}

static void test_clear_resets_everything_and_reuse() {
    std::cout << "\n== test_clear_resets_everything_and_reuse ==\n";
    AVLTree t;
    for (int i = 0; i < 50; ++i) t.insert(static_cast<ItemType>(i));
    EXPECT_EQ(t.count(), static_cast<size_t>(50));
    t.clear();
    EXPECT_EQ(t.count(), static_cast<size_t>(0));
    EXPECT_TRUE(t.inorder().empty());
    EXPECT_TRUE(t.preorder().empty());
    EXPECT_TRUE(t.postorder().empty());
    EXPECT_TRUE(t.minimumNode() == nullptr);
    EXPECT_TRUE(t.maximumNode() == nullptr);

    // clear on already empty is safe
    t.clear();
    EXPECT_EQ(t.count(), static_cast<size_t>(0));

    // reuse after clear
    std::vector<ItemType> again = { 3,1,4,2 };
    for (auto v : again) t.insert(v);
    auto want = again; std::sort(want.begin(), want.end());
    EXPECT_VEC_EQ(t.inorder(), want, "reuse-after-clear inorder");
    EXPECT_EQ(t.count(), again.size());
}

static void test_extreme_values() {
    std::cout << "\n== test_extreme_values ==\n";
    AVLTree t;
    ItemType a = static_cast<ItemType>(std::numeric_limits<int>::min());
    ItemType b = static_cast<ItemType>(-1);
    ItemType c = static_cast<ItemType>(0);
    ItemType d = static_cast<ItemType>(1);
    ItemType e = static_cast<ItemType>(std::numeric_limits<int>::max());
    std::vector<ItemType> vals = { c, d, b, e, a };
    for (auto v : vals) t.insert(v);

    auto in = t.inorder();
    auto want = vals; std::sort(want.begin(), want.end());
    EXPECT_VEC_EQ(in, want, "inorder with INT_MIN/INT_MAX/negatives/zero");

    expect_same_node(t.minimumNode(), t.find(a), "min == INT_MIN");
    expect_same_node(t.maximumNode(), t.find(e), "max == INT_MAX");
}

static void test_duplicates_observation() {
    std::cout << "\n== test_duplicates_observation ==\n";
    AVLTree t;
    std::vector<ItemType> base = { 10,20,30 };
    for (auto v : base) t.insert(v);

    size_t before = t.count();
    t.insert(20); // duplicate
    size_t after = t.count();

    if (after == before) {
        std::cout << "[INFO] Duplicate policy: IGNORE (count unchanged)\n";
    }
    else if (after == before + 1) {
        std::cout << "[INFO] Duplicate policy: ALLOW (count +1)\n";
        auto in = t.inorder();
        int dupCount = 0;
        for (auto v : in) if (v == static_cast<ItemType>(20)) ++dupCount;
        EXPECT_EQ(dupCount, 2);
    }
    else {
        std::cout << "[INFO] Duplicate policy: OTHER (count delta=" << (after - before) << ")\n";
    }
}

// ---------------- main ----------------
int main() {
    std::cout << "Running AVLTree tests (extended + nullptr coverage)…\n";
    try { test_empty_tree_basics_and_null_ok(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_empty_tree_basics_and_null_ok: " << e.what() << "\n"; failures++; }
    try { test_singleton_boundaries_and_null_again(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_singleton_boundaries_and_null_again: " << e.what() << "\n"; failures++; }
    try { test_insert_and_traversals_basic(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_insert_and_traversals_basic: " << e.what() << "\n"; failures++; }
    try { test_min_max_and_find(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_min_max_and_find: " << e.what() << "\n"; failures++; }
    try { test_next_smallest_largest_boundaries_and_null(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_next_smallest_largest_boundaries_and_null: " << e.what() << "\n"; failures++; }
    try { test_predecessor_successor_structural(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_predecessor_successor_structural: " << e.what() << "\n"; failures++; }
    try { test_rotations_patterns(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_rotations_patterns: " << e.what() << "\n"; failures++; }
    try { test_long_monotone_sequences(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_long_monotone_sequences: " << e.what() << "\n"; failures++; }
    try { test_copy_ctor_and_assignment(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_copy_ctor_and_assignment: " << e.what() << "\n"; failures++; }
    try { test_empty_copy_assign_selfassign(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_empty_copy_assign_selfassign: " << e.what() << "\n"; failures++; }
    try { test_clear_resets_everything_and_reuse(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_clear_resets_everything_and_reuse: " << e.what() << "\n"; failures++; }
    try { test_extreme_values(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_extreme_values: " << e.what() << "\n"; failures++; }
    try { test_duplicates_observation(); }
    catch (const std::exception& e) { std::cerr << "EXC in test_duplicates_observation: " << e.what() << "\n"; failures++; }

    if (failures == 0) {
        std::cout << "\nAll tests PASSED\n";
        return 0;
    }
    else {
        std::cerr << "\nTests finished with " << failures << " failure(s)\n";
        return 1;
    }
}
