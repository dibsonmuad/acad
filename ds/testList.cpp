#include "List.hpp"

#if HAVE_GTEST
#include <gtest/gtest.h>
class TestList : public ::testing : Test
{
protected:
    List<int> myList;
}

TEST_F(TestList, addElement)
{
    myList.add(10);
}

#else
int main()
{
    List<int> myList;
    myList.add(10);
    myList.print();
    myList.add(20);
    myList.reverse();
    myList.print();
}

#endif
