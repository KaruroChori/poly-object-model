/*
    Automatic sanity check procedure to see if the graphs and programs are working as expected.
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>

using namespace std;
namespace fs=std::filesystem;

int main(){
    try{
        string res_dir=getenv("RES_DIR");
        for(auto& p: fs::directory_iterator(res_dir+"test/graphs-runtime-checks")){
            cout<<"Testing ["<<p.path()<<"]\n";
            cout<<(string)p.path()+"/graph.json"<<"\n";
            ifstream graph_file((string)p.path()+"/graph.json");
            ifstream program_file((string)p.path()+"/program.json");
            ifstream target_file((string)p.path()+"/target.json");
            
            if(graph_file.fail()){cerr<<"Cannot open the graph file.\n";exit(1);}
            if(program_file.fail()){cerr<<"Cannot open the program file.\n";exit(1);}
            if(target_file.fail()){cerr<<"Cannot open the target file.\n";exit(1);}

            graph_file.close();
            program_file.close();
            target_file.close();
        }
    }
    catch(exception& ex){cerr<<ex.what()<<"\n";return 1;}
    catch(...){return 1;}
    return 0;
}