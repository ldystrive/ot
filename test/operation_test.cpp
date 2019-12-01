#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "operation.h"

using namespace std;

TEST(operation, addOp)
{
    Operation op;

    InsertOp insertA(string("abc"));
    RetainOp retainA(3);
    DeleteOp deleteA(2);
    InsertOp insertB(string("de"));
    int targetLength = insertA.length() + retainA.length() + insertB.length();

    Operation A = op.addInsert(insertA);
    Operation C = op.addRetain(retainA);
    Operation D = op.addDelete(deleteA);
    Operation B = op.addInsert(insertB);

    string ans = B.apply(string("czcdd"));
    EXPECT_EQ(ans, string("abcczcde"));
    
    EXPECT_EQ(ans.length(), targetLength);

}