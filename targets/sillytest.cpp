
#include <vector>
#include <iostream>

template <class TYPE>
class Better: public TYPE {
    public:
        size_t size() {
            return TYPE::size() + 1;
        }
};

using namespace std;

Better<vector<int>> vec;

int main(int argc, char** argv) {
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    std::cout << vec.size() << "\n";
}