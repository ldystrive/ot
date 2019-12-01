#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "operation.h"

using namespace std;

TEST(operation, addOp)
{
    Operation op;
    InsertOp insertA(string("abc"));
    InsertOp insertB(string("de"));

    Operation A = op.addInsert(insertA);
    Operation B = op.addInsert(insertB);
    string ans = B.apply(string(), 0);
    EXPECT_EQ(ans, string("abcde"));
    EXPECT_EQ(ans.length(), insertA.length() + insertB.length());
}