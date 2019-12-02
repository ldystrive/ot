#include <iostream>
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "operation.h"

using namespace std;


string genString(int len)
{
    string str;
    for (int i = 0; i < len; i++) {
        char base = 'a';
        if (rand() % 2) base = 'A';
        char ch = base + rand() % 26;
        str = str + ch;
    }
    return str;
}

Operation genOperation(int baseLength)
{
    Operation op;
    int index = 0;
    int insertCount = 0;
    for (int i = 0; i < rand() % 200 + 2; i++) {
        int t = rand() % 3;
        if (index == baseLength) t = 0;
        if (t == 0) { // insert
            string str = genString(rand() % 5);
            op.addInsert(InsertOp(str));
            insertCount += str.length();
        }
        else if (t == 1) { // delete
            int len = rand() % (baseLength - index);
            op.addDelete(DeleteOp(len));
            index += len;
        }
        else { // retain
            int len = rand() % (baseLength - index);
            op.addRetain(RetainOp(len));
            index += len;
        }
    }
    if (index != baseLength) {
        int t = rand() % 3 + 1;
        if (t == 1) { // delete
            op.addDelete(DeleteOp(baseLength - index));
        }
        else { // retain
            op.addRetain(RetainOp(baseLength - index));
        }
    }
    if (insertCount == 0) {
        string str = genString(rand() % 5);
        op.addInsert(InsertOp(str));
    }
    return op;
} 

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

TEST(operation, transform)
{
    for (int i = 0; i < 200; i++) {
        
        int baseLength = rand() % 3000;
        
        Operation A = genOperation(baseLength);
        Operation B = genOperation(baseLength);

        string baseStr = genString(baseLength);

        pair<Operation, Operation> newOps = Operation::transform(A, B);
        Operation newA = newOps.first;
        Operation newB = newOps.second;

        string ans1 = newB.apply(A.apply(baseStr));
        string ans2 = newA.apply(B.apply(baseStr));
        EXPECT_EQ(ans1, ans2);
        EXPECT_TRUE(ans1 != string(""));
        EXPECT_TRUE(ans2 != string(""));
    }

    

}