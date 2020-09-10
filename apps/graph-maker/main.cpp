#include <iostream>
#include <fstream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>
#include <regex>

using namespace smile::utils;
using namespace std;
using namespace nlohmann;

int main(int argc, const char* argv[]){
    std::map<string,graph<string,string>> graphs;
    for(;;){
        string buffer;
        getline(cin,buffer,'\n');

        to_edit_graph(graphs["banana"]);
        if(buffer=="exit")break;
        cout<<buffer;
    }


    try{
    }
    catch(std::exception& e){
        std::cout<<e.what();
    }
    return 0;
}
