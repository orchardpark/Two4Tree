#include <cstddef>
#include <vector>
#include <iostream>

using namespace std;
// Represents a data item (key value pair)
template <class K, class V>
class DataItem{
    public:
    DataItem(K k, V v){
        this->k = k;
        this->v = v;
    }
    K k;
    V v;
};

// Represents a node in the 2-3-4 tree
template<class K, class V>
class Node{
    public:
    Node(){
        child_array = vector<Node*>(ORDER, NULL);
        item_array = vector<DataItem<K,V>*>(ORDER-1, NULL);
        num_items = 0;
        num_items_children = 0;
    }
    ~Node(){
        for(Node<K,V>* child: child_array){
            delete child;
        }
        for(DataItem<K,V>* item : item_array){
            delete item;
        }
    }
    void insert_child(int child_num, Node* child){
        if(child!=NULL){
            if(child_array[child_num]==NULL) connect_child(child_num, child);
            else{
                for(int j=num_items; j>=child_num; j--){
                    child_array[j+1] = child_array[j];
                }
                connect_child(child_num, child);
            }
        }
    }
    void connect_child(int child_num, Node* child){
        if(child != NULL){
            child_array[child_num] = child;
            child->parent = this;
            int delta = child->num_items_children + child->num_items;
            this->num_items_children += delta;
            update_parent_item_count(delta);
        }
    }
    Node* disconnect_child(int child_num){
        Node* tmp = child_array[child_num];

        if(tmp != NULL) {
            int delta = (tmp->num_items_children + tmp->num_items);
            this->num_items_children -= delta;
            update_parent_item_count(-delta);
            tmp->parent = NULL;
        }
        child_array[child_num] = NULL;
        return tmp;
    }
    Node* get_child(int child_num){
        return child_array[child_num];
    }
    Node* get_parent(){
        return parent;
    }
    bool is_leaf(){
        return child_array[0] == NULL;
    }
    int get_num_items(){
        return num_items;
    }
    int get_num_items_children(){
        return num_items_children;
    }
    DataItem<K,V>* get_item(int index){
        return item_array[index];
    }
    bool is_full(){
        return num_items == ORDER - 1;
    }
    int find_item_index(K key){
        for(int j=0; j<ORDER-1; j++){
            if(item_array[j]==NULL) break;
            if(item_array[j]->k == key) return j;
        }
        return -1;
    }
    DataItem<K,V>* find_item(K key){
        int index = find_item_index(key);
        if(index == -1) return NULL;
        else return item_array[index];
    }
    int insert_item(DataItem<K,V>* new_item){
        num_items++;
        update_parent_item_count(1);
        K new_key = new_item->k;
        for(int j=ORDER-2; j>=0; j--){
            if(item_array[j] == NULL){
                continue;
            }
            else{
                K key = item_array[j]->k;
                if(new_key < key){
                    item_array[j+1] = item_array[j];
                }
                else{
                    item_array[j+1] = new_item;
                    return j+1;
                }
            }
        }
        item_array[0] = new_item;
        return 0;
    }
    void remove_item_at_index(int index){
        item_array[index] = NULL;
        for(int j=index; j<num_items-1; j++) {
            item_array[j] = item_array[j+1];
            item_array[j+1]=NULL;
        }
        for (int j = 0; j < num_items; j++) {
            if(child_array[j]==NULL) {
                child_array[j] = child_array[j + 1];
                child_array[j + 1] = NULL;
            }
        }
        num_items--;
        update_parent_item_count(-1);
    }
    DataItem<K, V>* remove_item(){
        DataItem<K, V>* temp = item_array[num_items-1];
        item_array[num_items-1] = NULL;
        num_items--;
        update_parent_item_count(-1);
        return temp;
    }
    private:
    void update_parent_item_count(int count){
        if(parent == NULL) return;
        parent->num_items_children += count;
        parent->update_parent_item_count(count);
    }
    static const int ORDER = 4;
    int num_items;
    int num_items_children;
    Node* parent;
    vector<Node*> child_array;
    vector<DataItem<K,V>*> item_array;
};

// Represents a 2-3-4 tree
template <class K, class V>
class Two4Tree{
    public:
    // constructors
    Two4Tree(){
        root = new Node<K, V>();
    }
    Two4Tree(K keys[], V values[], int s){
        root = new Node<K, V>();
        for(int j=0; j<s; j++) this->insert(keys[j], values[j]);
    }

    // destructor
    ~Two4Tree(){
        delete root;
    }
    // copy constructor
    Two4Tree(const Two4Tree& t){
        this->root = t.root;
    }
    // copy assignment operator
    Two4Tree& operator=(const Two4Tree& t){
        if(this == &t) return *this;
        this->root = t.root;
        return *this;
    }

    // public methods
    V* search(K key){
        Node<K,V>* curr_node = root;
        while(!curr_node->is_leaf()){
            DataItem<K,V>* item = curr_node->find_item(key);
            if(item != NULL) return &(item->v);
            else{
                curr_node = get_next_child(curr_node, key);
            }
        }
        DataItem<K,V>* item = curr_node->find_item(key);
        if(item == NULL) return NULL;
        else return &(item->v);
    }
    void insert(K key, V value){
        Node<K,V>* cur_node = root;
        auto tmp_item = new DataItem<K,V>(key, value);
        while(true){
            if(cur_node->is_full()){
                split(cur_node);
                cur_node = cur_node->get_parent();
                cur_node = get_next_child(cur_node, key);
            }
            else if(cur_node->is_leaf()){
                break;
            }
            else{
                cur_node = get_next_child(cur_node, key);
            }
        }
        cur_node->insert_item(tmp_item);

    }
    int remove(K key){
        bool has_key = search(key)!=NULL;
        if(!has_key) return 0;
        remove_rec(key, root);
        return 1;
    }
    int rank(K key){
        bool has_key = search(key)!=NULL;
        if(!has_key) return 0;
        return 1+rank_rec(key, root);
    }
    K select(int pos){
        return select_rec(pos-1, root);
    }
    K successor(K key){
        DataItem<K,V>* succ = successor_rec(key, root);
        if(succ == NULL) return NULL;
        else return succ->k;
    }
    K predecessor(K key){
        DataItem<K,V>* pred = predecessor_rec(key, root);
        if(pred == NULL) return NULL;
        else return pred->k;
    }
    int size(){
        return root->get_num_items()+root->get_num_items_children();
    }
    void preorder(){
        print_preorder(root);
        cout << endl;
    }
    void inorder(){
        print_inorder(root);
        cout << endl;
    }
    void postorder(){
        print_postorder(root);
        cout << endl;
    }
    void print_tree(){
        print_tree_rec(root, 0);
        cout << endl;
    }

    private:

    void print_tree_rec(Node<K,V>* node, int level){
        cout << "level " << level << ": ";
        if(node->is_leaf()){
            for (int i = 0; i < node->get_num_items(); i++) {
                cout << node->get_item(i)->k << "/";
            }
            cout << endl;
        }
        else {
            for (int i = 0; i < node->get_num_items(); i++) {
                cout << node->get_item(i)->k << "/";
            }
            cout << endl;
            for (int i = 0; i <= node->get_num_items(); i++) {
                print_tree_rec(node->get_child(i), level + 1);
            }
        }
    }

    bool rotate(Node<K,V>* node){
        Node<K,V>* parent = node->get_parent();
        int index = get_index_of_node_in_parent_child_array(node);

        if(index > 0){
            Node<K,V>* left_sibling = parent->get_child(index-1);
            if(left_sibling->get_num_items()>1){
                DataItem<K, V>* smallestKeyInRoot = parent->get_item(0);
                parent->remove_item_at_index(0);
                DataItem<K, V>* highestKeyInSibling = left_sibling->get_item(left_sibling->get_num_items()-1);
                Node<K,V>* child = left_sibling->disconnect_child(left_sibling->get_num_items());
                left_sibling->remove_item_at_index(left_sibling->get_num_items()-1);
                node->insert_item(smallestKeyInRoot);
                parent->insert_item(highestKeyInSibling);
                node->insert_child(0, child);
                return true;
            }
        }
        if(index < parent->get_num_items()){
            Node<K,V>* right_sibling = parent->get_child(index+1);
            if(right_sibling->get_num_items()>1){
                DataItem<K,V>* highestKeyInRoot = parent->get_item(parent->get_num_items()-1);
                parent->remove_item_at_index(parent->get_num_items()-1);
                DataItem<K,V>* smallestKeyInSibling = right_sibling->get_item(0);
                Node<K,V>* child = right_sibling->disconnect_child(0);
                right_sibling->remove_item_at_index(0);
                node->insert_item(highestKeyInRoot);
                parent->insert_item(smallestKeyInSibling);
                node->connect_child(node->get_num_items(), child);
                return true;
            }
        }
        return false;
    }

    bool merge(Node<K,V>* node){
        Node<K,V>* parent = node->get_parent();
        if(parent->get_parent()==NULL) return false;
        int index = get_index_of_node_in_parent_child_array(node);

        if(index < parent->get_num_items()){
            Node<K,V>* right_sibling = parent->get_child(index+1);

            auto new_node = new Node<K,V>();
            DataItem<K,V>* parent_item = parent->remove_item();
            DataItem<K,V>* node_item = node->get_item(0);
            DataItem<K,V>* sibling_item = right_sibling->get_item(0);
            new_node->insert_item(parent_item);
            new_node->insert_item(node_item);
            new_node->insert_item(sibling_item);

            parent->disconnect_child(index);
            parent->disconnect_child(index+1);
            parent->connect_child(index, new_node);
            new_node->connect_child(0, node->get_child(0));
            new_node->connect_child(1, node->get_child(1));
            new_node->connect_child(2, right_sibling->get_child(0));
            new_node->connect_child(3, right_sibling->get_child(1));

        } else{
            Node<K,V>* left_sibling = parent->get_child(index-1);
            auto new_node = new Node<K,V>();
            DataItem<K,V>* parent_item = parent->remove_item();
            DataItem<K,V>* node_item = node->get_item(0);
            DataItem<K,V>* sibling_item = left_sibling->get_item(0);
            new_node->insert_item(parent_item);
            new_node->insert_item(node_item);
            new_node->insert_item(sibling_item);

            parent->disconnect_child(index);
            parent->disconnect_child(index-1);
            parent->connect_child(index-1, new_node);
            new_node->connect_child(0, left_sibling->get_child(0));
            new_node->connect_child(1, left_sibling->get_child(1));
            new_node->connect_child(2, node->get_child(0));
            new_node->connect_child(3, node->get_child(1));

        }
        return true;
    }

    void shrink_tree(Node<K,V>* node){
        Node<K,V>* parent = node->get_parent();
        int index = get_index_of_node_in_parent_child_array(node);

        Node<K,V>* sibling;
        if(index==0) sibling = parent->get_child(1);
        else sibling = parent->get_child(0);
        auto new_root = new Node<K,V>();

        DataItem<K,V>* parent_item = parent->get_item(0);
        DataItem<K,V>* sibling_item = sibling->get_item(0);
        DataItem<K,V>* node_item = node->get_item(0);
        new_root->insert_item(parent_item);
        new_root->insert_item(sibling_item);
        new_root->insert_item(node_item);

        if(index == 0) {
            new_root->connect_child(0, node->get_child(0));
            new_root->connect_child(1, node->get_child(1));
            new_root->connect_child(2, sibling->get_child(0));
            new_root->connect_child(3, sibling->get_child(1));
        } else{
            new_root->connect_child(0, sibling->get_child(0));
            new_root->connect_child(1, sibling->get_child(1));
            new_root->connect_child(2, node->get_child(0));
            new_root->connect_child(3, node->get_child(1));
        }

        root = new_root;
    }

    void remove_rec(K key, Node<K,V>* node){
        if(node->is_leaf()){
            int index = node->find_item_index(key);
            node->remove_item_at_index(index);
        } else{
            if(node->get_num_items()==1 && node->get_parent() != NULL){
                if(rotate(node));
                else if(merge(node));
                else shrink_tree(node);
            }
            for(int j=0; j<node->get_num_items(); j++){
                DataItem<K,V>* item = node->get_item(j);
                if(item->k == key){
                    DataItem<K,V>* next = successor_rec(key, node);
                    if(next == NULL) {
                        node->remove_item_at_index(j);
                        node->disconnect_child(j+1);
                        return;
                    }else {
                        DataItem<K, V> tmp = *next;
                        *next = *item;
                        *item = tmp;
                    }
                } else if(item->k > key){
                    remove_rec(key, node->get_child(j));
                    return;
                }
            }
            remove_rec(key, node->get_child(node->get_num_items()));
        }
    }

    K select_rec(int pos, Node<K,V>* node){
        if(node->is_leaf()){
            return node->get_item(pos)->k;
        }
        else {
            for (int j = 0; j < node->get_num_items(); j++) {
                Node<K, V> *child = node->get_child(j);
                int child_count = child->get_num_items() + child->get_num_items_children();
                if (child_count > pos) return select_rec(pos, node->get_child(j));
                else{
                    pos -= child_count;
                    if(pos==0) return node->get_item(j)->k;
                    else pos--;
                }
            }
            return select_rec(pos, node->get_child(node->get_num_items()));
        }
    }

    int rank_rec(K key, Node<K, V>* node){
        int count = 0;
        if(node->is_leaf()){
            for(int j=0; j<node->get_num_items(); j++){
                if(node->get_item(j)->k < key) count++;
            }
            return count;
        } else {
            for(int j=0; j<node->get_num_items(); j++){
                if(node->get_item(j)->k > key) return count+rank_rec(key, node->get_child(j));
                else if(node->get_item(j)->k == key){
                    Node<K,V>* child = node->get_child(j);
                    return count+child->get_num_items_children()+child->get_num_items();
                }
                else{
                    Node<K,V>* child = node->get_child(j);
                    count += 1 + child->get_num_items() + child->get_num_items_children();
                }
            }
            count += rank_rec(key, node->get_child(node->get_num_items()));
            return count;
        }

    }

    DataItem<K,V>* predecessor_rec(K key, Node<K,V>* node){
        if(node->is_leaf()){
            for(int j=node->get_num_items()-1; j>=0; j--){
                if(node->get_item(j)->k < key) return node->get_item(j);
            }
            return NULL;
        }
        else{
            DataItem<K,V>* pred = predecessor_rec(key, node->get_child(node->get_num_items()));
            if(pred != NULL) return pred;
            for(int j=node->get_num_items()-1; j>=0; j--){
                if(node->get_item(j)->k < key) return node->get_item(j);
                DataItem<K,V>* pred = predecessor_rec(key, node->get_child(j));
                if(pred != NULL) return pred;
            }
            return NULL;
        }
    }

    DataItem<K,V>* successor_rec(K key, Node<K,V>* node){
        if(node->is_leaf()){
            for(int j=0; j<node->get_num_items(); j++){
                if(node->get_item(j)->k > key) return node->get_item(j);
            }
            return NULL;
        }
        else {
            for (int j = 0; j < node->get_num_items(); j++) {
                if (key < node->get_item(j)->k) {
                    DataItem<K,V>* succ = successor_rec(key, node->get_child(j));
                    if (succ != NULL) return succ;
                    else {
                        if(node->get_item(j)->k > key) return node->get_item(j);
                    }
                }
            }
            DataItem<K,V>* succ = successor_rec(key, node->get_child(node->get_num_items()));
            if (succ != NULL) return succ;
            return NULL;
        }
    }

    void print_inorder(Node<K,V>* node){
        if(node->is_leaf()){
            for(int j=0; j<node->get_num_items(); j++){
                cout << node->get_item(j)->k << " ";
            }
        }
        else {
            for (int j = 0; j < node->get_num_items(); j++) {
                print_inorder(node->get_child(j));
                cout << node->get_item(j)->k << " ";
            }
            print_inorder(node->get_child(node->get_num_items()));
        }
    }

    void print_preorder(Node<K,V>* node){
        if(node->is_leaf()){
            for (int j = 0; j < node->get_num_items(); j++) {
                cout << node->get_item(j)->k << " ";
            }
        }
        else {
            for (int j = 0; j < node->get_num_items(); j++) {
                cout << node->get_item(j)->k << " ";
            }
            for (int j = 0; j <= node->get_num_items(); j++) {
                print_preorder(node->get_child(j));
            }
        }
    }

    void print_postorder(Node<K,V>* node){
        if(node->is_leaf()){
            for (int j = 0; j < node->get_num_items(); j++) {
                cout << node->get_item(j)->k << " ";
            }
        } else {
            print_postorder(node->get_child(node->get_num_items()));
            for(int j=node->get_num_items()-1; j>=0; j--){
                cout << node->get_item(j)->k << " ";
                print_postorder(node->get_child(j));
            }
        }
    }

    void split(Node<K,V>* node){
        Node<K,V>* parent;
        int item_index;
        DataItem<K,V>* itemC = node->remove_item();
        DataItem<K,V>* itemB = node->remove_item();
        Node<K,V>* child2 = node->disconnect_child(2);
        Node<K,V>* child3 = node->disconnect_child(3);
        auto new_right = new Node<K,V>();
        if(node == this->root) {
            this->root = new Node<K, V>();
            parent = this->root;
            this->root->connect_child(0, node);
        } else {
            parent = node->get_parent();
        }
        item_index = parent->insert_item(itemB);
        int n = parent->get_num_items();
        for(int j=n-1; j>item_index; j--){
            Node<K,V>* tmp = parent->disconnect_child(j);
            parent->connect_child(j+1, tmp);
        }
        parent->connect_child(item_index+1, new_right);
        new_right->insert_item(itemC);
        new_right->connect_child(0, child2);
        new_right->connect_child(1, child3);
    }


    Node<K,V>* get_next_child(Node<K,V>* node, K key){
        int j;
        int num_items = node->get_num_items();
        for(j=0; j<num_items; j++){
            if(key < node->get_item(j)->k){
                return node->get_child(j);
            }
        }
        return node->get_child(j);
    }
    Node<K,V> *root;

    int get_index_of_node_in_parent_child_array(Node<K,V>* node){
        Node<K,V>* parent = node->get_parent();
        int index = 0;
        for(int j=0; j<=parent->get_num_items(); j++){
            if(parent->get_child(j)==node){
                index = j;
                break;
            }
        }
        return index;
    }
};