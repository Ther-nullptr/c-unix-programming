#include <iostream>
#include <deque>
#include <unordered_map>
#include <string>

#define WINDOW_SIZE 5 // for simplicity, use 5 instead of 25

bool is_word_repeated(const std::deque<std::string>& window, const std::string& word) {
    int count = 0;
    for (const auto& w : window) {
        if (w == word) {
            count++;
            if (count > 0) {
                return true;
            }
        }
    }
    return false;
}

int main() {
    std::string word;
    std::deque<std::string> window;

    std::cout << "input: " << std::endl;

    while (std::cin >> word) {
        if (is_word_repeated(window, word)) {
            std::cout << "word " << word << " repeat in the recent 5 words" << std::endl;
        }

        window.push_back(word);

        if (window.size() > WINDOW_SIZE) {
            window.pop_front();
        }
    }

    return 0;
}
