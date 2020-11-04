#ifndef __SINGLETON_H__
#define __SINGLETON_H__

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