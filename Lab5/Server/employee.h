#include <ostream>
struct employee {
    int num;
    char name[10];
    double hours;
    void print(std::ostream &out){
        out << "ID: " << num
        << "\tName: " << name
        << "\tHours: " << hours << std::endl;
    }
};

int empCmp(const void* p1, const void* p2){
    return ((employee*)p1)->num - ((employee*)p2)->num;
}
