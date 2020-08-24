#include <iostream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>

int main(){
    smile::utils::graph<uint64_t,uint64_t> hello;
    //hello.add_node(102);
    //hello.add_node(103);
    //hello.add_node(101);
    std::cout<<hello;

    smile::utils::edit_graph<uint64_t,uint64_t> banana;
    //banana.add_node({smile::utils::edit_node<uint64_t>::labels::ABSOLUTE,(void*)123});
    std::cout<<banana;
    return 0;
}
