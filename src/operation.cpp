#include <iostream>
#include <memory>

#include "operation.h"
using namespace std;

BasicOperation::BasicOperation(): len(0) {}
BasicOperation::BasicOperation(int len): len(len) {}
BasicOperation::~BasicOperation() {}
// OpType BasicOperation::getType() { return OpType::Other; }
int BasicOperation::length() const{ return len; }

InsertOp::InsertOp(): str("") {}
InsertOp::InsertOp(std::string str): BasicOperation(str.length()), str(str) {}
InsertOp::InsertOp(const InsertOp& A): InsertOp(A.getStr()) {}
InsertOp::~InsertOp() {}
OpType InsertOp::getType() { return OpType::Insert; }
std::string InsertOp::getStr() const{ return str; }
InsertOp InsertOp::operator + (const InsertOp &op) const{
    return InsertOp(getStr() + op.getStr());
}

RetainOp::RetainOp() {}
RetainOp::RetainOp(int len): BasicOperation(len) {}
RetainOp::RetainOp(const RetainOp& A): RetainOp(A.length()) {}
RetainOp::~RetainOp() {}
OpType RetainOp::getType() { return OpType::Retain; }
RetainOp RetainOp::operator + (const RetainOp &op) const{
    return RetainOp(length() + op.length());
}

DeleteOp::DeleteOp() {}
DeleteOp::DeleteOp(int len): BasicOperation(len) {}
DeleteOp::DeleteOp(const DeleteOp& A): DeleteOp(A.length()) {}
DeleteOp::~DeleteOp() {}
OpType DeleteOp::getType() { return OpType::Delete; }
DeleteOp DeleteOp::operator + (const DeleteOp &op) const{
    return DeleteOp(length() + op.length());
}


Operation::Operation(): baseLength(0), targetLength(0) {}
Operation::~Operation() {}

Operation Operation::addInsert(const InsertOp& op) {
    if (op.length() == 0) return *this;
    targetLength += op.length();
    if (!ops.empty() && ops[ops.size()-1]->getType() == OpType::Insert) {
        auto lastOp = dynamic_pointer_cast<InsertOp>(ops[ops.size()-1]);
        ops[ops.size()-1] = make_shared<InsertOp>((*lastOp) + op);
    }
    else if (ops.size() > 1 && ops[ops.size()-1]->getType() == OpType::Delete) {
        if (ops[ops.size()-2]->getType() == OpType::Insert) {
            auto lastOp = dynamic_pointer_cast<InsertOp>(ops[ops.size()-2]);
            ops[ops.size()-2] = make_shared<InsertOp>((*lastOp) + op);
        }
        else {
            ops.push_back(ops[ops.size()-1]);
            ops[ops.size()-2] = make_shared<InsertOp>(op);
        }
    }
    else {
        ops.push_back(make_shared<InsertOp>(op));
    }
    return *this;
}

std::string Operation::apply(std::string str, int len) {
    if (len != baseLength) {
        std::cerr << "The operation's base length must be equal to the string's length." << std::endl;        
        return std::string();
    }
    std::string newStr;
    // std::vector<std::string> newStr;
    int strIndex = 0;
    for (auto op : ops) {
        if (op->getType() == OpType::Retain) {
            if (strIndex + op->length() > len) {
                std::cerr << "Operation can't retain more characters than are left in the string." << std::endl;
                return std::string();
            }
            newStr = newStr + str.substr(strIndex, op->length());
            strIndex += op-> length();
        }
        else if (op->getType() == OpType::Insert) {
            newStr = newStr + dynamic_pointer_cast<InsertOp>(op)->getStr();
        }
        else if (op->getType() == OpType::Delete) {
            strIndex += op->length();
        }
    }

    if (newStr.length() != targetLength) {
        std::cerr << "The operation didn't operate on the whole string." << std::endl;
    }
    return newStr;
}
