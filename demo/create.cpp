//
// Created by aleksandr on 27.04.2021.
//
#include <Creator.hpp>
int main(int argc, const char *argv[]){
  Creator creator;
  if (argc > 1){
    creator.create_new_random_db(argv[1]);
  } else {
    creator.create_new_random_db("../testdb");
  }
}