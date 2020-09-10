#include <iostream>
#include <fstream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>
#include <utils/abstract-graph.h>

int main(int argc, const char* argv[]){
    bool normalize=false;

    if(argc!=3 && argc!=4){
        std::cout<<"Usage: "<<argv[0]<<" program graph [normalize=yes/no]\n";
        exit(1);
    }

    if(argc==4){
        if((std::string)argv[3]=="yes")normalize=true;
        else if((std::string)argv[3]=="no")normalize=false;
        else{
            std::cout<<"Usage: "<<argv[0]<<" program graph [normalize=yes/no]\n";
            exit(1);
        }
    }

    std::fstream program_file(argv[1]), graph_file(argv[2]);
    
    if(!program_file){
        std::cerr<<"Unable to open ["<<argv[1]<<"] as program file.\n";
        exit(1);
    }

    if(!graph_file){
        std::cerr<<"Unable to open ["<<argv[2]<<"] as graph file.\n";
        exit(1);
    }

    try{

        nlohmann::json program_json, graph_json;
        program_file>>program_json;
        graph_file>>graph_json;

        program_file.close();
        graph_file.close();

        smile::utils::graph<nlohmann::json,nlohmann::json> graph;
        smile::utils::edit_graph<nlohmann::json,nlohmann::json> program;

        smile::utils::abstract_graph<nlohmann::json,nlohmann::json> a_graph(graph);

        smile::utils::from_json(graph_json,graph);
        smile::utils::from_json(program_json,program);

        program.compile();
        program.apply_on(graph);
        std::cout<<graph;

        //graph.hash(graph.normalize());

    }
    catch(std::exception& e){
        std::cout<<e.what();
    }

    //Test of the poly library.
    poly::domain n1;
    return 0;
}
