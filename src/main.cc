#include <cstdlib>
#include <string>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;

#include <omp.h>
#include <sw2v.h>

#ifndef LOCAL
#include "ps/ps.h"

void StartServer() {
  if (!ps::IsServer()) return;
  auto server = new ps::KVServer<float>(0);
  server->set_request_handle(ps::KVServerDefaultHandle<float>());
  ps::RegisterExitCallback([server](){ delete server; });
}
#endif

int main(int argc, char ** argv) {
  /*
  string port = ps::Environment::Get()->find("PORT");
  cout << "port: " << port << endl;
  port = ps::Environment::Get()->find("PORT0");
  cout << "port: " << port << endl;
  */
  //string port = "3333";
  //cout << "port: " << port << endl;
#ifndef LOCAL
  StartServer();
  ps::Start();
  if (ps::IsWorker()) {
#endif
    //string root = ps::Environment::Get()->find("SW2V_DATA");
    string root = "/export/sw2v";
    cout << "root: " << root << endl;
    srand(time(NULL));
    sw2v::SparseWord2Vec algo(100, 5, 128, 0.005);
    algo.LoadVocab((root + "/term_dict").c_str());

#if LOCAL
    int rank = 0;
#else
    int rank = ps::MyRank();
#endif
    for(int i = 0; i < 100; i++) {
      string fname = root + "/term_id_list_" + to_string(rank);
      cout << fname << endl;
      DataIter iter(fname.c_str());
      algo.Train(iter);
      cout << "epoc " << i << endl;
      if (rank == 0 && i%10 == 0) {       
        string mfile = root + "/term_id_list_model_epoc_" + to_string(i);
        algo.SaveModel(mfile.c_str());
      }
    }
#ifndef LOCAL
  }
  
  ps::Finalize();
#endif
  cout << "finished" << endl;
  /*
  while(true) {
    this_thread::sleep_for(chrono::seconds(5));
    cout << "sleep" << endl;
  }
  */
}
