#include <iostream>

#include "pstring.h"
#include "test_pstring_a.h"
#include "test_pstring_w.h"

void PStringATest() {
    PStringATestAssignment();
    PStringATestOperator();
    PStringASubStringTest();
    PStringAChangecaseReplaceSplitJoinTest();
}

void PStringWTest() {
    PStringWTestAssignment();
    PStringWTestOperator();
    PStringWSubStringTest();
    PStringWChangecaseReplaceSplitJoinTest();
}

int main(int argc, char** argv) {
    PStringWTest();
}