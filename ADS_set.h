#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
    //using key_compare = std::less<key_type>;                         // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;                              // Hashing

 private:
    class Element{
        key_type key;
        Element* next;
    public:
        Element(key_type key, Element* next = nullptr): key(key), next(next){}
        const key_type& get_key() const{
            return key;
        }

        Element* get_next() const{
            return next;
        }

        void set_next(Element* new_next) {
            this->next = new_next;
        }

        ~Element(){
            delete next;
        }

    };

    Element** table;

    size_type capacity = N;

    size_type current_size = 0;

    void resize(size_type new_capacity) {
        Element **new_table = new Element*[new_capacity]{};
        for (size_type i {0}; i < capacity; i++)
        {
            Element* current = table[i];
            while (current != nullptr)
            {
                size_type newindex = hasher{}(current->get_key()) % new_capacity;
                Element* new_current = new_table[newindex];
                if (new_current == nullptr)
                    new_table[newindex] = new Element (current->get_key());
                else{
                    Element* new_first = new Element (current->get_key(), new_current);
                    new_table[newindex] = new_first;
                }
                current = current->get_next();
            }
        }
        for (size_type i {0}; i < capacity; i++)
        {
            delete table[i];
        }
        capacity = new_capacity;
        delete[] table;
        table = new_table;
    }


    void add(key_type key){
        if (current_size >= 0.7*capacity)
            resize(2*capacity);
        size_type index = hasher{}(key) % capacity;
        Element* current = table[index];
        if (current == nullptr){
            table[index] = new Element(key);
            current_size++;
        }
        else{
            Element* new_first = new Element(key, current);
            table[index] = new_first;
            current_size++;
        }
    }



public:



    size_type get_capacity() const{
    	return capacity;
    }

    ADS_set(){
        table = new Element*[N]{};
    }
    ADS_set(std::initializer_list<key_type> ilist):ADS_set{}{
        insert(ilist);
    }

    template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{}{
        insert(first, last);
    }

    ADS_set(const ADS_set &other)
    {
        table = new Element*[other.capacity]{};
        for (size_type i {0}; i < other.capacity; i++){
            Element* current = other.table[i];
            while (current != nullptr){
                add(current->get_key());
                current = current->get_next();
            }
        }
    }

    ~ADS_set(){
        for (size_type i {0}; i < capacity; i++){
            delete table[i];
        }
        delete[] table;
    }

    Element** get_table() const{
        return table;
    }

    size_type size() const{
        return current_size;
    }

    bool empty() const{
        return size() == 0;
    }

    void insert(std::initializer_list<key_type> ilist){
        insert(ilist.begin(), ilist.end());
    }

    template<typename InputIt> void insert(InputIt first, InputIt last){
        for (auto it{first}; it != last; ++it)
        {
            if (!count(*it))
                add(*it);
        }
    }

    size_type count(const key_type &key) const{
        size_type index = hasher{}(key) % capacity;
        Element* current = table[index];
        while (current != nullptr){
            if (key_equal{}(current->get_key(), key))
                return 1;
            current = current->get_next();
        }
        return 0;
    }



    //PH2

    std::pair<iterator,bool> insert(const key_type &key){
        size_type index = hasher{}(key) % capacity;
        Element* current = table[index];
        if (current == nullptr){
            add(key);
            return {iterator{this, table[hasher{}(key) % capacity], hasher{}(key) % capacity}, true};
        }
        else{
            while (current != nullptr)
            {
                if (key_equal{}(key,current->get_key()))
                    return {iterator{this, current, index}, false};
                current = current->get_next();
            }
            add(key);
            return {iterator{this, table[hasher{}(key) % capacity], hasher{}(key) % capacity}, true};
    	 }
    }

    void clear(){
        ADS_set temp {};
        swap(temp);
    }

    size_type erase(const key_type &key) {
        if(find(key)==end()) return 0;
        size_type index = hasher{}(key) % capacity;
        Element* current = table[index];
        Element* prev = nullptr;
        while (current != nullptr) {
            if (key_equal{}(current->get_key(), key)) {
                if(prev == nullptr)
                {
                    if(current->get_next() == nullptr)
                    {
                        table[index] = nullptr;
                    }
                    else
                    {
                        table[index] = current->get_next();
                        current->set_next(nullptr);
                    }
                }
                else
                {
                    if (current->get_next() == nullptr)
                    {
                        prev->set_next(nullptr);
                    }
                    else
                    {
                        prev->set_next(current->get_next());
                        current->set_next(nullptr);
                    }
                }
                delete current;
                --current_size;
                return 1;
            }
            prev = current;
            current = current->get_next();
        }
        return 0;
    }

    iterator find(const key_type &key) const{
        size_type index = hasher{}(key) % capacity;
        Element* current = table[index];
        while (current != nullptr){
            if (key_equal{}(current->get_key(), key))
                return iterator{this, current, index};
            current = current->get_next();
        }
        return end();
    }

    void swap(ADS_set &other){
        std::swap(table, other.table);
        std::swap(capacity, other.capacity);
        std::swap(current_size, other.current_size);
    }

    const_iterator begin() const{
        return const_iterator{this, table[0],0};
    }

    const_iterator end() const{
        return const_iterator{this, nullptr, capacity, true};
    }

    ADS_set &operator=(const ADS_set &other){
        ADS_set temp{other};
        swap(temp);
        return *this;
    }

    ADS_set &operator=(std::initializer_list<key_type> ilist){
        ADS_set temp{ilist};
        swap(temp);
        return *this;
    }

    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs){
        if (lhs.current_size != rhs.current_size)
            return false;
        for (auto i {lhs.begin()}; i != lhs.end(); ++i){
            if (!rhs.count(*i))
                return false;
        }
        return true;
    }

    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs){
        return !(lhs==rhs);
    }

    void dump(std::ostream &o = std::cerr) const{
        o << "table capacity: " << capacity << ", current size: " << current_size << "\n";
        for (size_t i {0}; i < capacity; i++){
            Element* current = table[i];
            if (current == nullptr)
                continue;
            o << "index " << i << ": ";
            while (current != nullptr){
                o << current->get_key() << " ";
                current = current->get_next();
            }
            o << "\n";
        }
    }

};

template <typename Key, size_t N>

class ADS_set<Key,N>::Iterator {
    const ADS_set *set {nullptr};
    Element* pos;
    size_type table_idx;
    bool end {false};

    void skip(){
        if (end) return;
        if (set->size() == 0) return;
        while (pos == nullptr && table_idx < set->get_capacity())
        {
            ++table_idx;
            pos = set->get_table()[table_idx];
            if(table_idx == set->get_capacity())
            	pos = nullptr;
        }
    }

    void chain_skip(){
        if (end) return;
        if (set->size() == 0) return;
        if (pos == nullptr) return;
        if (pos->get_next() != nullptr) pos = pos->get_next();
        else
        {
            pos = pos->get_next();
            skip();
        }
    }

public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator(const ADS_set *set, Element* pos, size_type table_idx = 0, bool fend = false): set(set), pos(pos), table_idx(table_idx),end(fend)
    {
        if (!end)
        skip();
    }

    explicit Iterator(){};

    reference operator*() const{
        return pos->get_key();
    }

    pointer operator->() const{
        return &pos->get_key();
    }

    Iterator &operator++(){
        if (pos == nullptr)
            skip();
        else
            chain_skip();
        return *this;
    }

    Iterator operator++(int){
        auto rc{*this};
        ++*this;
        return rc;
    }


    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
        return lhs.pos == rhs.pos;
    }

    friend bool operator!=(const Iterator &lhs, const Iterator &rhs){
        return !(lhs == rhs);
    }
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) {
    lhs.swap(rhs);
}


#endif // ADS_SET_H