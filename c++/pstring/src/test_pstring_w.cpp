#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "pstring.h"
#include "test_pstring_w.h"

void PStringWTestAssignment() {
    PStringW hello = L"hello";
    PStringW world = L"world";
    std::wstring stlwstr = L"this is stlwstr";
    PStringW convert_stlwstr = stlwstr;

    printf("<PStringWTestAssignment>\n");

    printf("Using printf:\n");
    printf("\t%S %S\n", hello.cStr(), world.cStr());
    printf("\t%S\n", convert_stlwstr.cStr());

    std::wcout << L"Using std::wcout" << std::endl;
    std::wcout << L"\t" << hello << " " << world << std::endl;
    std::wcout << L"\t" << convert_stlwstr << std::endl;
    printf("\n");
}

void PStringWTestOperator() {
    PStringW empty;
    PStringW left = L"left";
    PStringW alphabets = L"abcdefghijklmnopqrstuvwxyz";
    std::vector<PStringW> to_sort;

    printf("<PStringWTestOperator>\n");

    printf("\"%S\" == \"%S\" : %d\n", left.cStr(), L"left", (left == L"left"));
    printf("\"%S\" == \"%S\" : %d\n", left.cStr(), L"lefty", (left == L"lefty"));
    printf("\"%S\" == \"%S\" : %d\n", left.cStr(), L"lef", (left == L"lef"));
    printf("\"%S\" == \"%S\" : %d\n", left.cStr(), L"lef", (left == L"lef"));
    printf("\"%S\" == \"%S\" : %d\n", empty.cStr(), L"lef", (empty == L"lef"));
    printf("\"%S\" == \"%S\" : %d\n", empty.cStr(), L"", (empty == L""));

    printf("\"%S\" != \"%S\" : %d\n", left.cStr(), L"left", (left != L"left"));
    printf("\"%S\" != \"%S\" : %d\n", left.cStr(), L"lefty", (left != L"lefty"));
    printf("\"%S\" != \"%S\" : %d\n", left.cStr(), L"lef", (left != L"lef"));
    printf("\"%S\" != \"%S\" : %d\n", left.cStr(), L"lef", (left != L"lef"));
    printf("\"%S\" != \"%S\" : %d\n", empty.cStr(), L"lef", (empty != L"lef"));
    printf("\"%S\" != \"%S\" : %d\n", empty.cStr(), L"", (empty != L""));

    to_sort.push_back(L"Do not go gentle into that good night,");
    to_sort.push_back(L"Old age should burn and rave at close of day");
    to_sort.push_back(L"Rage, rage against the dying of the light.");
    to_sort.push_back(L"Though wise men at their end know dark is right,");
    to_sort.push_back(L"Because their words had forked no lightning they");
    to_sort.push_back(L"Do not go gentle into that good night.");
    to_sort.push_back(L"Good men, the last wave by, crying how bright");
    to_sort.push_back(L"Their frail deeds might have danced in a green bay,");
    to_sort.push_back(L"Rage, rage against the dying of the light.");
    to_sort.push_back(L"Wild men who caught and sang the sun in flight,");
    to_sort.push_back(L"And learn, too late, they grieved it on its way,");
    to_sort.push_back(L"Do not go gentle into that good night.");
    to_sort.push_back(L"Grave men, near death, who see with blinding sight");
    to_sort.push_back(L"Blind eyes could blaze like meteorsand be gay,");
    to_sort.push_back(L"Rage, rage against the dying of the light.");
    to_sort.push_back(L"And you, my father, there on the sad height,");
    to_sort.push_back(L"Curse, bless, me now with your fierce tears, I pray.");
    to_sort.push_back(L"Do not go gentle into that good night.");
    to_sort.push_back(L"Rage, rage against the dying of the light.");

    printf("Before sort:\n");
    for (auto& s : to_sort) {
        printf("\t%S\n", s.cStr());
    }
    printf("\n");

    std::sort(to_sort.begin(), to_sort.end());

    printf("After sort:\n");
    for (auto& s : to_sort) {
        printf("\t%S\n", s.cStr());
    }
    printf("\n");

    printf("Access each char:\n");
    for (int i = 0; i < alphabets.length(); ++i) {
        printf("alphabets[%d] : %C\n", i, alphabets[i]);
    }
    printf("\n");

    printf("\n");
}

void PStringWSubStringTest() {
    PStringW test_str = L"0123456789";

    printf("<PStringWSubStringTest>\n");

    std::wcout << L"test_str = " << test_str << std::endl;
    std::wcout << L"test_str.substr(0,5) = " << test_str.substr(0, 5) << std::endl;
    std::wcout << L"test_str.substr(3,7) = " << test_str.substr(3, 7) << std::endl;
    std::wcout << L"test_str.substr(7,3) = " << test_str.substr(7, 3) << std::endl;
    std::wcout << L"test_str.substr(7,10) = " << test_str.substr(7, 10) << std::endl;
    std::wcout << L"test_str.substr(10,20) = " << test_str.substr(20, 30) << std::endl;
    std::wcout << L"test_str.substrFrom(2) = " << test_str.substrFrom(2) << std::endl;
    std::wcout << L"test_str.substrFrom(10) = " << test_str.substrFrom(10) << std::endl;
    std::wcout << L"test_str.substrTo(6) = " << test_str.substrTo(6) << std::endl;
    std::wcout << L"test_str.substrTo(20) = " << test_str.substrTo(20) << std::endl;
    std::wcout << L"test_str.substrSlice(3,8) = " << test_str.substrSlice(3, 8) << std::endl;
    std::wcout << L"test_str.substrSlice(5,5) = " << test_str.substrSlice(5, 5) << std::endl;
    std::wcout << L"test_str.substrSlice(7,4) = " << test_str.substrSlice(7, 4) << std::endl;
    std::wcout << L"test_str.substrSlice(2,20) =" << test_str.substrSlice(2, 20) << std::endl;

    printf("\n");
}

void PStringWChangecaseReplaceSplitJoinTest() {
    PStringW text = \
        L"Do not go gentle into that good night,\n"
        L"Old age should burn and rave at close of day\n"
        L"Rage, rage against the dying of the light.\n"
        L"Though wise men at their end know dark is right,\n"
        L"Because their words had forked no lightning they\n"
        L"Do not go gentle into that good night.\n"
        L"Good men, the last wave by, crying how bright\n"
        L"Their frail deeds might have danced in a green bay,\n"
        L"Rage, rage against the dying of the light.\n"
        L"Wild men who caught and sang the sun in flight,\n"
        L"And learn, too late, they grieved it on its way,\n"
        L"Do not go gentle into that good night.\n"
        L"Grave men, near death, who see with blinding sight\n"
        L"Blind eyes could blaze like meteorsand be gay,\n"
        L"Rage, rage against the dying of the light.\n"
        L"And you, my father, there on the sad height,\n"
        L"Curse, bless, me now with your fierce tears, I pray.\n"
        L"Do not go gentle into that good night.\n"
        L"Rage, rage against the dying of the light.\n";
    
    PStringW text_lower;
    PStringW text_upper;
    std::vector<PStringW> text_upper_words;
    std::vector<PStringW> text_upper_lines;
    PStringW text_joined;

    printf("<PStringReplaceSplitJoinTest>");

    printf("text:\n");
    printf("%S\n", text.cStr());

    text_lower = text.lower();
    printf("text_lower = test.lower():\n");
    printf("%S\n", text_lower.cStr());

    printf("text_lower.replaceAll(\"gentle\",\"$!GENTLE@#\"):\n");
    printf("%S\n", text_lower.replaceAll(L"gentle", L"$!GENTLE@#").cStr());

    text_upper = text.upper(); // not text_lower.upper()!
    printf("text_upper = text.upper():\n");
    printf("%S\n", text_upper.cStr());

    text_upper.split(&text_upper_words);
    printf("text_upper.split result :\n");
    for (int i = 0; i < text_upper_words.size(); ++i) {
        printf("\t[%d] : \"%S\"\n", i, text_upper_words[i].cStr());
    }

    text_upper.split(L"\n", &text_upper_lines);
    printf("text_upper.split(\"\\n\") :\n");
    for (int i = 0; i < text_upper_lines.size(); ++i) {
        printf("\t[%d] : \"%S\"\n", i, text_upper_lines[i].cStr());
    }

    text_joined = PStringW::join(text_upper_lines, L"<EOL>\n");
    printf("PStringW::join(text_upper_lines, \"<EOL>\\n\"):\n");
    printf("%S\n", text_joined.cStr());
}