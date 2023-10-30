#include <iostream>
#include <array>
#include <random>
#include <bitset>
#include <unordered_map>

const unsigned int arr_size = 2500;
const unsigned int pokker_block_amount = arr_size * 2;
const unsigned int  pokker_block_length = 4;
class TestResult {
public:
    bool monobit;
    bool series;
    bool pokker;

    TestResult() {
        monobit = false;
        series = false;
        pokker = false;
    }
};

struct SeriesInfo {
    int max_zero_counter;
    int max_one_counter;
    int actual_zero_counter;
    int actual_one_counter;
    bool actual_bit;

    SeriesInfo() {
        max_one_counter = 0;
        max_zero_counter = 0;
        actual_zero_counter = 0;
        actual_one_counter = 0;
        actual_bit = 0;
    }
};

unsigned char generate_random_byte() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> byte_distribution(0, 255);
    return static_cast<unsigned char>(byte_distribution(gen));
}

std::array<unsigned char, arr_size> generate_char_arr() {
    std::array<unsigned char, arr_size> byte_arr{};
    for(int i = 0; i <arr_size; i++) {
        byte_arr[i]= generate_random_byte();
    }
    return byte_arr;
}

TestResult test_cypher(std::array<unsigned char, arr_size> byte_arr) {
    unsigned int monobit_byte_counter = 0;
    TestResult test_result{};
    SeriesInfo series_info{};
    std::unordered_map<std::bitset<4>, unsigned int> matches;
    const unsigned int pokker_sequence_amount = pow(2, pokker_block_length);
    for (int i = 0; i < arr_size; i++) {
        unsigned char byte = byte_arr[i];
        for (int j = 7; j >= 0; j--) {
            bool bit = (byte >> j) & 1;
            //Monobyte test
            if(bit) ++monobit_byte_counter;

            //Series test
            if(bit == series_info.actual_bit) {
                if(bit) {
                    series_info.actual_one_counter++;
                    if(series_info.actual_one_counter > series_info.max_one_counter) {
                        series_info.max_one_counter = series_info.actual_one_counter;
                    }
                } else {
                    series_info.actual_zero_counter++;
                    if(series_info.actual_zero_counter > series_info.max_zero_counter) {
                        series_info.max_zero_counter = series_info.actual_zero_counter;
                    }
                }
            } else {
                series_info.actual_bit = bit;
                if(bit) {
                    series_info.actual_one_counter++;
                    series_info.actual_zero_counter = 0;
                } else {
                    series_info.actual_zero_counter++;
                    series_info.actual_one_counter = 0;
                }
            }
        }
        //Pokker test
        std::bitset<4> byte_splitted_by_tetras[2] = {byte >> 4, byte & 0b1111};
        for (auto byte_splitted_by_tetra : byte_splitted_by_tetras) {
            auto match_it = matches.find(byte_splitted_by_tetra);
            if (match_it != matches.end()) {
                match_it->second++;
            } else {
                matches[byte_splitted_by_tetra] = 1;
            }
        }
    }
    unsigned long int pokker_counter = 0;
    std::vector<unsigned long long int> pokker_values = {};
    for (const auto& pair : matches) {
        pokker_values.push_back(pair.second);
    }
    for(unsigned int i = 0; i < pokker_sequence_amount; i++) {
        unsigned int value;
        if (i < pokker_values.size()) {
            value = pokker_values[i];
        } else {
            value = 0;
        }
        pokker_counter += pow(value, 2);
    }
    double result = (static_cast<double>(pokker_sequence_amount) / static_cast<double>(pokker_block_amount))
                    * pokker_counter - pokker_block_amount;

    if((result > 1.03)&& (result < 57.13)) {
        test_result.pokker = true;
    }

    if((monobit_byte_counter >= 9654) && (monobit_byte_counter <= 10362)) {
        test_result.monobit = true;
    }

    if(series_info.max_one_counter <= 36 && series_info.max_zero_counter <= 36) {
        test_result.series = true;
    }
    return test_result;
}

int main() {
    std::array<unsigned char, arr_size> byte_arr = generate_char_arr();
    TestResult test_result = test_cypher(byte_arr);
    std::cout<<"Monobyte's test result: "<<(test_result.monobit ? "passed": "failed")<<std::endl;
    std::cout<<"Series' test result: "<<(test_result.series ? "passed": "failed")<<std::endl;
    std::cout<<"Pokker's test result: "<<(test_result.pokker ? "passed": "failed")<<std::endl;
    return 0;
}