#include <iostream>
#include <memory>
#include <exception>
#include <utility>

#include "operation.h"
using namespace std;

BasicOperation::BasicOperation(): len(0) {}
BasicOperation::BasicOperation(int len): len(len) {}
BasicOperation::~BasicOperation() {}
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

Operation Operation::clone() const{
    Operation newop;
    newop.baseLength = baseLength;
    newop.targetLength = targetLength;
    for (auto op : ops) {
        if (op->getType() == OpType::Insert) {
            newop.ops.push_back(make_shared<InsertOp>(*dynamic_pointer_cast<InsertOp>(op)));
        }
        else if (op->getType() == OpType::Delete) {
            newop.ops.push_back(make_shared<DeleteOp>(*dynamic_pointer_cast<DeleteOp>(op)));
        }
        else {
            newop.ops.push_back(make_shared<RetainOp>(*dynamic_pointer_cast<RetainOp>(op)));
        }
    }
    return newop;
}

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

Operation Operation::addRetain(const RetainOp& op) {
    if (op.length() == 0) return *this;
    baseLength += op.length();
    targetLength += op.length();

    if (!ops.empty() && ops[ops.size()-1]->getType() == OpType::Retain) {
        auto lastOp = dynamic_pointer_cast<RetainOp> (ops[ops.size()-1]);
        ops[ops.size()-1] = make_shared<RetainOp>(op + (*lastOp));
    }
    else {
        ops.push_back(make_shared<RetainOp>(op));
    }
    return *this;
}

Operation Operation::addDelete(const DeleteOp& op) {
    if (op.length() == 0) return *this;
    baseLength += op.length();
    if (!ops.empty() && ops[ops.size()-1]->getType() == OpType::Delete) {
        auto lastOp = dynamic_pointer_cast<DeleteOp> (ops[ops.size()-1]);
        ops[ops.size()-1] = make_shared<DeleteOp>(op+(*lastOp));
    }
    else {
        ops.push_back(make_shared<DeleteOp>(op));
    }
    return *this;
}

std::string Operation::apply(std::string str) {
    if (str.length() != baseLength) {     
        throw logic_error("The operation's base length must be equal to the string's length.");  
    }
    std::string newStr;
    int strIndex = 0;
    for (auto op : ops) {
        if (op->getType() == OpType::Retain) {
            if (strIndex + op->length() > str.length()) {
                throw logic_error("Operation can't retain more characters than are left in the string.");
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
        throw logic_error("The operation didn't operate on the whole string.");
    }
    return newStr;
}

bool Operation::isNoop() {
    return ops.size() == 0 || (ops.size() == 1 && ops[0]->length() == 0);
}


pair<Operation,Operation> Operation::transform(const Operation &A, const Operation &B) {
    if (A.baseLength != B.baseLength) {
        throw logic_error("Both operations have to have the same base length");
    }
    Operation operation1prime;
    Operation operation2prime;
    Operation copyA = A.clone();
    Operation copyB = B.clone();
    auto ops1 = copyA.ops;
    auto ops2 = copyB.ops;
    int i1 = 0, i2 = 0;
    auto op1It = ops1.begin();
    auto op2It = ops2.begin();

    while (true) {
        if (op1It == ops1.end() && op2It == ops2.end()) {
            break;
        }
        // cout << "op1 " << (op1It != ops1.end() ? (*op1It)->length() : -1);
        // cout << " op2 " << (op2It != ops2.end() ? (*op2It)->length() : -1) << endl;
        // one or both ops are insert ops
        if (op1It != ops1.end() && (*op1It)->getType() == OpType::Insert) {
            auto op1 = *op1It;
            operation1prime.addInsert(*dynamic_pointer_cast<InsertOp>(op1));
            operation2prime.addRetain(RetainOp(op1->length()));
            op1It++;
            continue;
        }
        if (op2It != ops2.end() && (*op2It)->getType() == OpType::Insert) {
            auto op2 = *op2It;
            operation1prime.addRetain(RetainOp(op2->length()));
            operation2prime.addInsert(*dynamic_pointer_cast<InsertOp>(op2));
            op2It++;
            continue;
        }

        if (op1It == ops1.end()) {
            throw logic_error("Cannot compose operations: first operation is too short.");
        }
        if (op2It == ops2.end()) {
            throw logic_error("Cannot compose operations: first operation is too long.");
        }

        auto op1 = *op1It;
        auto op2 = *op2It;
        int min1 = 0;
        if (op1->getType() == OpType::Retain && op2->getType() == OpType::Retain) {
            if (op1->length() > op2->length()) {
                min1 = op2->length();
                *op1It = make_shared<RetainOp>(RetainOp(op1->length() - op2->length()));
                op2It++;               
            }
            else if (op1->length() == op2->length()) {
                min1 = op1->length();
                op1It++;
                op2It++;
            }
            else {
                min1 = op1->length();
                *op2It = make_shared<RetainOp>(RetainOp(op2->length() - op1->length()));
                op1It++;
            }
            operation1prime.addRetain(RetainOp(min1));
            operation2prime.addRetain(RetainOp(min1));
        }
        else if (op1->getType() == OpType::Delete && op2->getType() == OpType::Delete) {
            if (op1->length() > op2->length()) {
                *op1It = make_shared<DeleteOp>(DeleteOp(op1->length() - op2->length()));
                op2It++;
            }
            else if (op1->length() == op2->length()) {
                // TODO: skip both ops?
                op1It++;
                op2It++;
            }
            else {
                *op2It = make_shared<DeleteOp>(DeleteOp(op2->length() - op1->length()));
                op1It++;
            }
        }
        else if (op1->getType() == OpType::Delete  && op2->getType() == OpType::Retain) {
            if (op1->length() > op2->length()) {
                min1 = op2->length();
                *op1It = make_shared<DeleteOp>(DeleteOp(op1->length() - op2->length()));
                op2It++;
            }
            else if (op1->length() == op2->length()) {
                min1 = op1->length();
                op1It++;
                op2It++;
            }
            else {
                min1 = op1->length();
                *op2It = make_shared<RetainOp>(RetainOp(op2->length() - op1->length()));
                op1It++;
            }
            operation1prime.addDelete(DeleteOp(min1));
        }
        else if (op1->getType() == OpType::Retain && op2->getType() == OpType::Delete) {
            if (op1->length() > op2->length()) {
                min1 = op2->length();
                *op1It = make_shared<RetainOp>(RetainOp(op1->length() - op2->length()));
                op2It++;
            }
            else if (op1->length() == op2->length()) {
                min1 = op1->length();
                op1It++;
                op2It++;
            }
            else {
                min1 = op1->length();
                *op2It = make_shared<DeleteOp>(DeleteOp(op2->length() - op1->length()));
                op1It++;
            }
            operation2prime.addDelete(DeleteOp(min1));
        }
        else {
            throw logic_error("The two operations aren't compatible");
        }
    }
    return {operation1prime, operation2prime};
}

bool Operation::operator == (const Operation& A) const{
    if (ops.size() != A.ops.size() || baseLength != A.baseLength || targetLength != A.targetLength) {
        return false;
    }

    for (int i = 0; i < ops.size(); i++) {
        auto op1 = ops[i];
        auto op2 = A.ops[i];
        if (op1->length() != op2->length() || op1->getType() != op2->getType()) {
            return false;
        }
        if (op1->getType() == OpType::Insert) {
            string str1 = dynamic_pointer_cast<InsertOp>(op1)->getStr();
            string str2 = dynamic_pointer_cast<InsertOp>(op2)->getStr();
            if (str1 != str2) {
                return false;
            }
        } 
    }
    return true;
}

string Operation::toString() {
    string str;
    for (auto op : ops) {
        if (op->getType() == OpType::Retain) {
            str = str + "retain" + to_string(op->length());
        }
        else if (op->getType() == OpType::Insert) {
            str = str + "insert'";
            str = str + dynamic_pointer_cast<InsertOp>(op)->getStr() + "'";
        }
        else {
            str = str + "delete" + to_string(op->length());
        }
        str = str + ',';
    }
    return str;
}

Operation Operation::invert(string str)
{
    Operation inverse;
    int strIndex = 0;
    if (str.length() != baseLength) {
        throw length_error("baseLength and str's length are not equal.");
    }
    for (auto op : ops) {
        if (op->getType() == OpType::Retain) {
            inverse.addRetain(RetainOp(op->length()));
            strIndex += op->length();
        }
        else if (op->getType() == OpType::Insert) {
            inverse.addDelete(DeleteOp(op->length()));
        }
        else {
            inverse.addInsert(InsertOp(str.substr(strIndex, op->length())));
            strIndex += op->length();
        }
    }
    return inverse;
}

Operation Operation::operator + (const Operation &A) const {
    if (targetLength != A.baseLength) {
        throw length_error("The base length of the second operation has to be the target length of the first operation");
    }

    Operation operation;
    Operation copyOp1 = this->clone();
    Operation copyOp2 = A.clone();
    auto ops1 = copyOp1.ops;
    auto ops2 = copyOp2.ops;
    auto ops1It = ops1.begin();
    auto ops2It = ops2.begin();
    while (true) {
        if (ops1It == ops1.end() && ops2It == ops2.end()) {
            break;
        }
        
        if (ops1It != ops1.end() && (*ops1It)->getType() == OpType::Delete) {
            operation.addDelete(DeleteOp((*ops1It)->length()));
            ops1It++;
            continue;
        }
        if (ops2It != ops2.end() && (*ops2It)->getType() == OpType::Insert) {
            operation.addInsert(*dynamic_pointer_cast<InsertOp>(*ops2It));
            ops2It++;
            continue;
        }

        if (ops1It == ops1.end()) {
            throw logic_error("Cannot compose operations: first operation is too short.");
        }
        if (ops2It == ops2.end()) {
            throw logic_error("Cannot compose operations: first operation is too long.");
        }

        auto op1 = *ops1It;
        auto op2 = *ops2It;
        if (op1->getType() == OpType::Retain && op2->getType() == OpType::Retain) {
            if (op1->length() > op2->length()) {
                operation.addRetain(RetainOp(op2->length()));
                *ops1It = make_shared<RetainOp>(RetainOp(op1->length() - op2->length()));
                ops2It++;
            }
            else if (op1->length() == op2->length()) {
                operation.addRetain(RetainOp(op2->length()));
                ops1It++;
                ops2It++;
            }
            else {
                operation.addRetain(RetainOp(op1->length()));
                *ops2It = make_shared<RetainOp>(RetainOp(op2->length() - op1->length()));
                ops1It++;
            }
        }
        else if (op1->getType() == OpType::Insert && op2->getType() == OpType::Delete) {
            if (op1->length() > op2->length()) {
                InsertOp tmp = *dynamic_pointer_cast<InsertOp>(op1);
                *ops1It = make_shared<InsertOp>(InsertOp(tmp.getStr().substr(op2->length())));
                ops2It++;
            }
            else if (op1->length() == op2->length()) {
                ops1It++;
                ops2It++;
            }
            else {
                *ops2It = make_shared<DeleteOp>(DeleteOp(op2->length() - op1->length()));
                ops1It++;
            }
        }
        else if (op1->getType() == OpType::Insert && op2->getType() == OpType::Retain) {
            if (op1->length() > op2->length()) {
                InsertOp tmp = *dynamic_pointer_cast<InsertOp>(op1);
                operation.addInsert(InsertOp(tmp.getStr().substr(0, op2->length())));
                *ops1It = make_shared<InsertOp>(InsertOp(tmp.getStr().substr(op2->length())));
                ops2It++;
            }
            else if (op1->length() == op2->length()) {
                InsertOp tmp = *dynamic_pointer_cast<InsertOp>(op1);
                operation.addInsert(InsertOp(tmp));
                ops1It++;
                ops2It++;
            }
            else {
                InsertOp tmp = *dynamic_pointer_cast<InsertOp>(op1);
                operation.addInsert(InsertOp(tmp));
                *ops2It = make_shared<RetainOp>(RetainOp(op2->length() - op1->length()));
                ops1It++;
            }
        }
        else if (op1->getType() == OpType::Retain && op2->getType() == OpType::Delete) {
            if (op1->length() > op2->length()) {
                operation.addDelete(DeleteOp(op2->length()));
                *ops1It = make_shared<RetainOp>(RetainOp(op1->length() - op2->length()));
                ops2It++;
            }
            else if (op1->length() == op2->length()) {
                operation.addDelete(DeleteOp(op2->length()));
                ops1It++;
                ops2It++;
            }
            else {
                operation.addDelete(DeleteOp(op1->length()));
                *ops2It = make_shared<DeleteOp>(DeleteOp(op2->length() - op1->length()));
                ops1It++;
            }
        }
        else {
            auto type2str = [] (OpType x) {
                if (x == OpType::Delete) return string("Delete");
                if (x == OpType::Retain) return string("Retain");
                if (x == OpType::Insert) return string("Insert");
            };
            string str = string("this shouldn't happen: op1: ") + type2str(op1->getType());
            str = str + ", op2: " + type2str(op2->getType());
            throw logic_error(str);
        }
    }
    return operation;
}
