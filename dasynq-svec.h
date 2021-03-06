#ifndef DASYNQ_SVEC_H_INCLUDED
#define DASYNQ_SVEC_H_INCLUDED

#include <limits>
#include <utility>
#include <new>

// Vector with possibility to shrink capacity arbitrarily

namespace dasynq {

template <typename T>
class svector
{
    private:
    T * array;
    size_t size_v;
    size_t capacity_v;

    void check_capacity()
    {
        if (size_v == capacity_v) {
            // double capacity now:
            if (capacity_v == 0) capacity_v = 1;
            T * new_array = new T[capacity_v * 2];
            for (size_t i = 0; i < size_v; i++) {
                new (&new_array[i]) T(std::move(array[i]));
                array[i].T::~T();
            }
            delete[] array;
            array = new_array;
            capacity_v *= 2;
        }
    }

    public:
    using size_type = size_t;

    svector() : array(nullptr), size_v(0), capacity_v(0)
    {

    }

    svector(const svector<T> &other)
    {
        capacity_v = other.size_v;
        size_v = other.size_v;
        array = new T[capacity_v];
        for (size_t i = 0; i < size_v; i++) {
            new (&array[i]) T(other[i]);
        }
    }

    ~svector()
    {
        for (size_t i = 0; i < size_v; i++) {
            array[i].T::~T();
        }
        delete[] array;
    }

    void push_back(const T &t)
    {
        check_capacity();
        new (&array[size_v]) T(t);
        size_v++;
    }

    void push_back(T &&t)
    {
        check_capacity();
        new (&array[size_v]) T(t);
        size_v++;
    }

    template <typename ...U>
    void emplace_back(U... args)
    {
        check_capacity();
        new (&array[size_v]) T(args...);
        size_v++;
    }

    void pop_back()
    {
        size_v--;
    }

    T &operator[](size_t index)
    {
        return array[index];
    }

    const T &operator[](size_t index) const
    {
        return array[index];
    }

    size_t size() const
    {
        return size_v;
    }

    size_t capacity() const
    {
        return capacity_v;
    }

    bool empty() const
    {
        return size_v == 0;
    }

    static size_t max_size() noexcept
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);

        // if we were to support allocators:
        //size_t max = std::allocator_traits<std::allocator<char>>::max_size(std::allocator<T>());
        //return max / sizeof(T);
        //  (but not / sizeof(T) for C++17 apparently)
    }

    void reserve(size_t amount)
    {
        if (capacity_v < amount) {
            T * new_array = new T[amount];
            for (size_t i = 0; i < size_v; i++) {
                new (&new_array[i]) T(std::move(array[i]));
                array[i].T::~T();
            }
            delete[] array;
            array = new_array;
            capacity_v = amount;
        }
    }

    void shrink_to(size_t amount)
    {
        if (capacity_v > amount) {
            T * new_array = new(std::nothrow) T[amount];
            if (new_array == nullptr) {
                return;
            }
            for (size_t i = 0; i < size_v; i++) {
                new (&new_array[i]) T(std::move(array[i]));
                array[i].T::~T();
            }
            delete[] array;
            array = new_array;
            capacity_v = amount;
        }
    }

    T &back()
    {
        return array[size_v - 1];
    }

    T* begin()
    {
        return array;
    }

    const T *begin() const
    {
        return array;
    }

    T* end()
    {
        return array + size_v;
    }

    const T *end() const
    {
        return array + size_v;
    }
};


} // namespace

#endif
