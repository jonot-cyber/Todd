#include "monitor.h"

extern "C" {
    int main(void* mboot) {
        Monitor m;
        m.clear();
        m << "Hello, ToddOS!\n";
    }
}