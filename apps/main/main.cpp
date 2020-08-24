#include <iostream>
#include <fstream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>

int main(int argc, const char* argv[]){
    if(argc!=3){
        std::cout<<"Usage: "<<argv[0]<<" program graph\n";
        exit(1);
    }

    std::fstream program_file(argv[1]), graph_file(argv[2]);
    
    if(!program_file){
        std::cerr<<"Unable to open ["<<argv[1]<<"] as program file\n.";
        exit(1);
    }

    if(!graph_file){
        std::cerr<<"Unable to open ["<<argv[2]<<"] as graph file\n.";
        exit(1);
    }

    nlohmann::json program_json, graph_json;
    program_file>>program_json;
    graph_file>>graph_json;

    smile::utils::graph<uint64_t,uint64_t> graph;
    smile::utils::edit_graph<uint64_t,uint64_t> program;

    smile::utils::from_json(graph_json,graph);
    //smile::utils::from_json(program_json,program);

    program.apply_on(graph);
    //std::cout<<graph;

    return 0;
}
