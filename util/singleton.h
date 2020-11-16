#ifndef SUDOKU_UTIL_SINGLETON_H_
#define SUDUKU_UTIL_SINGLETON_H_

#include <util/global.h>

template <typename T>
class Singleton
{
    public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }

    protected:
        Singleton() {}
        ~Singleton() {}

    private:
        DISALLOW_CLASS_COPY_AND_ASSIGN(Singleton);
};

#endif	// __SINGLETON_H__