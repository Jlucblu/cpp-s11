#include <cassert>
#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
    T* ptr_;
public:
    UniquePtr() {
        prt_ = nullptr;
    }

    explicit UniquePtr(T* ptr) {

    }
    UniquePtr(const UniquePtr&) {

    }

    UniquePtr(UniquePtr&& other) {

    }

    UniquePtr& operator=(const UniquePtr&) {

    }

    UniquePtr& operator=(nullptr_t) {

    }

    UniquePtr& operator=(UniquePtr&& other) {

    }

    ~UniquePtr() {
        delete prt_;
    }

    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return prt_;
    }

    T* Release() {

    }

    void Reset(T* ptr) {

    }

    void Swap(UniquePtr& other) {
        swap(ptr_, other.ptr_);
    }

    T* Get() const {

    }
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v)
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value)
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }
    assert(Item::counter == 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
}