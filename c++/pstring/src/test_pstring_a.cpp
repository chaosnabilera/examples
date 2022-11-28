#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "pstring.h"
#include "test_pstring_a.h"

void PStringATestAssignment() {
    PStringA hello = "hello";
    PStringA world = "world";
    std::string stlstr = "this is stlstr";
    PStringA convert_stlstr = stlstr;

    printf("<PStringATestAssignment>\n");

    printf("Using printf:\n");
    printf("\t%s %s\n", hello.cStr(), world.cStr());
    printf("\t%s\n", convert_stlstr.cStr());

    std::cout << "Using std::cout" << std::endl;
    std::cout << "\t" << hello << " " << world << std::endl;
    std::cout << "\t" << convert_stlstr << std::endl;
    printf("\n");
}

void PStringATestOperator() {
    PStringA empty;
    PStringA left = "left";
    PStringA alphabets = "abcdefghijklmnopqrstuvwxyz";
    std::vector<PStringA> to_sort;

    printf("<PStringATestOperator>\n");
    
    printf("\"%s\" == \"%s\" : %d\n", left.cStr(), "left", (left == "left"));
    printf("\"%s\" == \"%s\" : %d\n", left.cStr(), "lefty", (left == "lefty"));
    printf("\"%s\" == \"%s\" : %d\n", left.cStr(), "lef", (left == "lef"));
    printf("\"%s\" == \"%s\" : %d\n", left.cStr(), "lef", (left == "lef"));
    printf("\"%s\" == \"%s\" : %d\n", empty.cStr(), "lef", (empty == "lef"));
    printf("\"%s\" == \"%s\" : %d\n", empty.cStr(), "", (empty == ""));

    printf("\"%s\" != \"%s\" : %d\n", left.cStr(), "left", (left != "left"));
    printf("\"%s\" != \"%s\" : %d\n", left.cStr(), "lefty", (left != "lefty"));
    printf("\"%s\" != \"%s\" : %d\n", left.cStr(), "lef", (left != "lef"));
    printf("\"%s\" != \"%s\" : %d\n", left.cStr(), "lef", (left != "lef"));
    printf("\"%s\" != \"%s\" : %d\n", empty.cStr(), "lef", (empty != "lef"));
    printf("\"%s\" != \"%s\" : %d\n", empty.cStr(), "", (empty != ""));

    to_sort.push_back("Do not go gentle into that good night,");
    to_sort.push_back("Old age should burn and rave at close of day");
    to_sort.push_back("Rage, rage against the dying of the light.");
    to_sort.push_back("Though wise men at their end know dark is right,");
    to_sort.push_back("Because their words had forked no lightning they");
    to_sort.push_back("Do not go gentle into that good night.");
    to_sort.push_back("Good men, the last wave by, crying how bright");
    to_sort.push_back("Their frail deeds might have danced in a green bay,");
    to_sort.push_back("Rage, rage against the dying of the light.");
    to_sort.push_back("Wild men who caughtand sang the sun in flight,");
    to_sort.push_back("And learn, too late, they grieved it on its way,");
    to_sort.push_back("Do not go gentle into that good night.");
    to_sort.push_back("Grave men, near death, who see with blinding sight");
    to_sort.push_back("Blind eyes could blaze like meteorsand be gay,");
    to_sort.push_back("Rage, rage against the dying of the light.");
    to_sort.push_back("And you, my father, there on the sad height,");
    to_sort.push_back("Curse, bless, me now with your fierce tears, I pray.");
    to_sort.push_back("Do not go gentle into that good night.");
    to_sort.push_back("Rage, rage against the dying of the light.");

    printf("Before sort:\n");
    for (auto& s : to_sort) {
        printf("\t%s\n", s.cStr());
    }
    printf("\n");

    std::sort(to_sort.begin(), to_sort.end());

    printf("After sort:\n");
    for (auto& s : to_sort) {
        printf("\t%s\n", s.cStr());
    }
    printf("\n");

    printf("Access each char:\n");
    for (int i = 0; i < alphabets.length(); ++i) {
        printf("alphabets[%d] : %c\n", i, alphabets[i]);
    }
    printf("\n");
    
    printf("\n");
}

void PStringASubStringTest() {
    PStringA test_str = "0123456789";

    printf("<PStringASubStringTest>\n");

    std::cout << "test_str = " << test_str << std::endl;
    std::cout << "test_str.substr(0,5) = "      << test_str.substr(0, 5) << std::endl;
    std::cout << "test_str.substr(3,7) = "      << test_str.substr(3, 7) << std::endl;
    std::cout << "test_str.substr(7,3) = "      << test_str.substr(7, 3) << std::endl;
    std::cout << "test_str.substr(7,10) = "     << test_str.substr(7, 10) << std::endl;
    std::cout << "test_str.substr(10,20) = "    << test_str.substr(20, 30) << std::endl;
    std::cout << "test_str.substrFrom(2) = "    << test_str.substrFrom(2) << std::endl;
    std::cout << "test_str.substrFrom(10) = "   << test_str.substrFrom(10) << std::endl;
    std::cout << "test_str.substrTo(6) = "      << test_str.substrTo(6) << std::endl;
    std::cout << "test_str.substrTo(20) = "     << test_str.substrTo(20) << std::endl;
    std::cout << "test_str.substrSlice(3,8) = " << test_str.substrSlice(3, 8) << std::endl;
    std::cout << "test_str.substrSlice(5,5) = " << test_str.substrSlice(5, 5) << std::endl;
    std::cout << "test_str.substrSlice(7,4) = " << test_str.substrSlice(7, 4) << std::endl;
    std::cout << "test_str.substrSlice(2,20) =" << test_str.substrSlice(2, 20) << std::endl;

    printf("\n");
}

void PStringAChangecaseReplaceSplitJoinTest() {
    PStringA text = \
        "Do not go gentle into that good night,\n"
        "Old age should burn and rave at close of day\n"
        "Rage, rage against the dying of the light.\n"
        "Though wise men at their end know dark is right,\n"
        "Because their words had forked no lightning they\n"
        "Do not go gentle into that good night.\n"
        "Good men, the last wave by, crying how bright\n"
        "Their frail deeds might have danced in a green bay,\n"
        "Rage, rage against the dying of the light.\n"
        "Wild men who caught and sang the sun in flight,\n"
        "And learn, too late, they grieved it on its way,\n"
        "Do not go gentle into that good night.\n"
        "Grave men, near death, who see with blinding sight\n"
        "Blind eyes could blaze like meteorsand be gay,\n"
        "Rage, rage against the dying of the light.\n"
        "And you, my father, there on the sad height,\n"
        "Curse, bless, me now with your fierce tears, I pray.\n"
        "Do not go gentle into that good night.\n"
        "Rage, rage against the dying of the light.\n";
    PStringA text_lower;
    PStringA text_upper;
    std::vector<PStringA> text_upper_words;
    std::vector<PStringA> text_upper_lines;
    PStringA text_joined;
    
    printf("<PStringReplaceSplitJoinTest>");

    printf("text:\n");
    printf("%s\n", text.cStr());

    text_lower = text.lower();
    printf("text_lower = test.lower():\n");
    printf("%s\n", text_lower.cStr());
    
    printf("text_lower.replaceAll(\"gentle\",\"$%GENTLE@#\"):\n");
    printf("%s\n", text_lower.replaceAll("gentle", "$%GENTLE@#").cStr());

    text_upper = text.upper(); // not text_lower.upper()!
    printf("text_upper = text.upper():\n");
    printf("%s\n", text_upper.cStr());

    text_upper.split(&text_upper_words);
    printf("text_upper.split result :\n");
    for (int i = 0; i < text_upper_words.size(); ++i) {
        printf("\t[%d] : \"%s\"\n", i, text_upper_words[i].cStr());
    }

    text_upper.split("\n", &text_upper_lines);
    printf("text_upper.split(\"\\n\") :\n");
    for (int i = 0; i < text_upper_lines.size(); ++i) {
        printf("\t[%d] : \"%s\"\n", i, text_upper_lines[i].cStr());
    }
    
    text_joined = PStringA::join(text_upper_lines, "<EOL>\n");
    printf("PStringA::join(text_upper_lines, \"<EOL>\\n\"):\n");
    printf("%s\n", text_joined.cStr());
}