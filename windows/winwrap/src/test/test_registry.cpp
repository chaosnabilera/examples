#include "winwrap_registry.h"

void WWTestRegistryEnum();
void WWTestCreateDelete();

void WWTestRegistry() {
    WWTestCreateDelete();
}

void WWTestRegistryExist() {
    WinRegistryA reg_a(WinRegistryA::WOW6464KEY);
    std::string path_1 = "HKEY_CURRENT_USER\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartupApproved\\Run";
    std::string path_2 = "HKCU\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartupApproved\\Run";
    std::string path_3 = "HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\ACPI";

    printf("test 1: %s : %d\n", path_1.c_str(), reg_a.keyExist(path_1));
    printf("test 2: %s : %d\n", path_2.c_str(), reg_a.keyExist(path_2));
    printf("test 3: %s : %d\n", path_3.c_str(), reg_a.keyExist(path_3));
}

void WWTestRegistryEnum() {
    WinRegistryA reg_a(WinRegistryA::WOW6464KEY);
    std::string epath_1 = "HKCU";
    std::vector<std::string> subkeys;

    reg_a.enumKey(epath_1, &subkeys, true, true);

    for (size_t i = 0; i < subkeys.size(); i++) {
        printf("subkey: %s\n", subkeys[i].c_str());
    }
}

void WWTestCreateDelete() {
    WinRegistryA reg_a(WinRegistryA::WOW6464KEY);
    
    std::vector<std::string> create_list;
    /*
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe2");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe3");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe4");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1\\hoho1");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1\\hoho2");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1\\hoho3");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1\\hoho4");
    create_list.push_back("HKCU\\Software\\Microsoft\\Windows\\haha\\hehe1\\hoho5");
    */
    create_list.push_back("HKCU\\haha");
    create_list.push_back("HKCU\\single_example");
    create_list.push_back("HKCU\\haha\\hehe1");
    create_list.push_back("HKCU\\haha\\hehe2");
    create_list.push_back("HKCU\\haha\\hehe3");
    create_list.push_back("HKCU\\haha\\hehe4");
    create_list.push_back("HKCU\\haha\\hehe1\\hoho1");
    create_list.push_back("HKCU\\haha\\hehe1\\hoho2");
    create_list.push_back("HKCU\\haha\\hehe1\\hoho3");
    create_list.push_back("HKCU\\haha\\hehe1\\hoho4");
    create_list.push_back("HKCU\\haha\\hehe1\\hoho5");
    

    for (size_t i = 0; i < create_list.size(); i++) {
        bool created_new;
        if (reg_a.createKey(create_list[i], &created_new)) {
            printf("create: %s : %d\n", create_list[i].c_str(), created_new);
        }
        else {
            printf("failed : %s\n", create_list[i].c_str());
        }
    }

    if (reg_a.removeKey(create_list[0], true)) {
        printf("removed : %s\n", create_list[0].c_str());
    }
    else {
        printf("remove failed : %s\n", create_list[0].c_str());
    }

    if (reg_a.removeKey(create_list[1], false)) {
        printf("removed : %s\n", create_list[1].c_str());
    }
    else {
        printf("remove failed : %s\n", create_list[1].c_str());
    }
}