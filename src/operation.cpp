// #include <iostream>

#include "operation.h"

BasicOperation::BasicOperation(): len(0) {}
BasicOperation::BasicOperation(int len): len(len) {}
BasicOperation::~BasicOperation() {}
OpType BasicOperation::getType() { return OpType::Other; }
int BasicOperation::length() const{ return len; }

InsertOp::InsertOp(): str("") {}
InsertOp::InsertOp(int len, std::string str): BasicOperation(len), str(str) {}
InsertOp::~InsertOp() {}
OpType InsertOp::getType() { return OpType::Insert; }
std::string InsertOp::getStr() const{ return str; }
InsertOp InsertOp::operator + (const InsertOp &op) const{
    return InsertOp(length() + op.length(), getStr() + op.getStr());
}

RetainOp::RetainOp() {}
RetainOp::RetainOp(int len): BasicOperation(len) {}
RetainOp::~RetainOp() {}
OpType RetainOp::getType() { return OpType::Retain; }
RetainOp RetainOp::operator + (const RetainOp &op) const{
    return RetainOp(length() + op.length());
}

DeleteOp::DeleteOp() {}
DeleteOp::DeleteOp(int len): BasicOperation(len) {}
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
    if (!ops.empty() && ops[ops.size()-1].getType() == OpType::Insert) {
        InsertOp& lastOp = dynamic_cast<InsertOp&>(ops[ops.size()-1]);
        ops[ops.size()-1] = lastOp + op;
    }
    else if (ops.size() > 1 && ops[ops.size()-1].getType() == OpType::Delete) {
        if (ops[ops.size()-2].getType() == OpType::Insert) {
            ops[ops.size()-2] = dynamic_cast<InsertOp&>(ops[ops.size()-2]) + op;
        }
        else {
            ops.push_back(ops[ops.size()-1]);
            ops[ops.size()-2] = op;
        }
    }
    else {
        ops.push_back(op);
    }
    return *this;
}

