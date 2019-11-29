#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "operation.h"

using namespace std;

TEST(operation, addOp)
{
    Operation op;
    InsertOp insertA(3, string("abc"));
    InsertOp insertB(2, string("de"));

    Operation A = op.addInsert(insertA);
    Operation B = op.addInsert(insertB);
}