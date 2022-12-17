#include "winwrap_registry.h"

void WWTestRegistryEnum();
void WWTestCreateDelete();
void WWTestEnumValue();
void WWTestSetGetEnumValue();

void WWTestRegistry() {
    WWTestSetGetEnumValue();
    WWTestEnumValue();
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

void WWTestEnumValue() {
    WinRegistryA reg_a(WinRegistryA::WOW6464KEY);
    std::string test_root = "HKCU\\test_setgetenum";
    std::vector<std::string> value_names;
    std::vector<WinRegistryValueA> values;

    do {
        if (!reg_a.enumValueName(test_root, &value_names)) {
            printf("reg_a.enumValueName %s failed\n", test_root.c_str());
            break;
        }
        printf("value names of %s:\n", test_root.c_str());
        for (auto& s : value_names) {
            printf("\t%s\n", s.c_str());
        }

        if (!reg_a.enumValue(test_root, &values)) {
            printf("reg_a.enumValue %s failed\n", test_root.c_str());
            break;
        }
        printf("value of %s:\n", test_root.c_str());
        for (auto& v : values) {
            printf("%s: %s\n", v.name.c_str(), v.descriptionString().c_str());
        }
    } while (0);
}

void WWTestSetGetEnumValue() {
    WinRegistryA reg_a(WinRegistryA::WOW6464KEY);
    std::string test_root = "HKCU\\test_setgetenum";
    WinRegistryValueA va;
    
    do {
        if (reg_a.keyExist(test_root)) {
            if (!reg_a.removeKey(test_root, true)) {
                printf("reg_a.removeKey %s failed\n", test_root.c_str());
                break;
            }
            printf("Removed prexisting key : %s\n", test_root.c_str());
        }

        if (!reg_a.createKey(test_root)) {
            printf("reg_a.createKey %s failed\n", test_root.c_str());
            break;
        }
        
        if (!reg_a.setValueDWORD(test_root, "vdword", 0x12345678)) {
            printf("reg_a.setValueDWORD(vdword, 0x12345678) failed\n");
            break;
        }
        if (!reg_a.getValue(test_root, "vdword", &va)) {
            printf("reg_a.getValue vdword failed\n");
            break;
        }
        printf("vdword : %s\n", va.descriptionString().c_str());

        if (!reg_a.setValueQWORD(test_root, "vqword", 0x123456789abcdef0)){
            printf("reg_a.setValueQWORD(vqword, 0x123456789abcdef0) failed\n");
            break;
        }
        if (!reg_a.getValue(test_root, "vqword", &va)) {
            printf("reg_a.getValue vqword failed\n");
            break;
        }
        printf("vqword : %s\n", va.descriptionString().c_str());

        if (!reg_a.setValueSZ(test_root, "vstring", "test string")) {
            printf("reg_a.setValueSZ(vstring, test string) failed\n");
            break;
        }
        if (!reg_a.getValue(test_root, "vstring", &va)) {
            printf("reg_a.getValue vstring failed\n");
            break;
        }
        printf("vstring : %s\n", va.descriptionString().c_str());

        if (!reg_a.setValueExpandSZ(test_root, "vexpstring", "test expand string")) {
            printf("reg_a.setValueExpandSZ(vexpstring, test_expand_string) failed\n");
            break;
        }
        if (!reg_a.getValue(test_root, "vexpstring", &va)) {
            printf("reg_a.getValue vexpstring failed\n");
            break;
        }
        printf("vexpstring : %s\n", va.descriptionString().c_str());

        if (!reg_a.setValueMultiSZ(test_root, "vmulti", { "test string1", "test string2", "test string3" })) {
            printf("reg_a.setValueMultiSZ(vmulti, {test string1, test string2, test string3}) failed\n");
            break;
        }
        if (!reg_a.getValue(test_root, "vmulti", &va)) {
            printf("reg_a.getValue vmulti failed\n");
            break;
        }
        printf("vmulti : %s\n", va.descriptionString().c_str());
        
    } while (0);
    
}