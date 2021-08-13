// istreambuf_iterator example
#include <fstream>     // std::cin, std::cout
#include <iostream>     // std::cin, std::cout
#include <iterator>     // std::istreambuf_iterator
#include <string>       // std::string
#include <vector>       // std::string

size_t read_with_it_assign(std::ifstream & file) {
    std::istreambuf_iterator<char> eos;        // end-of-range iterator
    std::istreambuf_iterator<char> iit (file); // stdin iterator

    std::string buff;
    buff.assign(iit, eos);
    return buff.size();
}

size_t read_with_it_loop(std::ifstream & file) {
    std::istreambuf_iterator<char> eos;        // end-of-range iterator
    std::istreambuf_iterator<char> iit (file); // stdin iterator

    std::string buff;
    while (iit != eos) buff.push_back(*iit++);

    return buff.size();
}

size_t read_char_by_char_on_string(std::ifstream & file, size_t sizeRead) {
    std::string buff;
    char c;
    file.get(c);
    for (size_t i = 0; file.good() && i < sizeRead - 1; i++) {
        buff.push_back(c);
        file.get(c);
    }
    return buff.size();
}

size_t read_char_by_char_on_vector(std::ifstream & file, size_t sizeRead) {
    std::vector<char> buff;
    char c;
    file.get(c);
    for (size_t i = 0; file.good() && i < sizeRead - 1; i++) {
        buff.push_back(c);
        file.get(c);
    }
    return buff.size();
}

// time ./test 1 ; time ./test 2 ; time ./test 3; time ./test 4

int main (int ac, char **av) {
    std::ifstream file("./file.log");
    if (ac > 1) {
        int val = atoi(av[1]);

        switch (val) {

            case 1:
                std::cout << "read_char_by_char_on_string: " << read_char_by_char_on_string(file, SIZE_T_MAX) << std::endl;
                break;
            case 2:
                std::cout << "read_char_by_char_on_vector: " << read_char_by_char_on_vector(file, SIZE_T_MAX) << std::endl;
                break;
            case 3:
                std::cout << "read_with_it_assign: " << read_with_it_assign(file) << std::endl;
                break;
            case 4:
                std::cout << "read_with_it_loop: " << read_with_it_loop(file) << std::endl;
                break;
        }
    }

    return 0;
}