#include <iostream>

#include "signal.hpp"

struct Test {
  Signal< const char * > printStr;
  Signal< > printNone;
};

struct Do {
  void printStr( const char *str ) {
    std::cout << "En funcion miembro: " << str << '\n';
  }
  void printNone( ) {
    std::cout << "printNone en funcion miembro\n";
  }
};

static void printStr( const char *str ) {
  std::cout << "En funcion suelta: " << str << '\n';
}

static void printNone( ) {
  std::cout << "printNone como funcion suelta\n";
}

int main( ) {
  Test test;
  Do imp;


  test.printStr.connect( &imp, &Do::printStr );
  test.printStr.connect( printStr );

  test.printStr( "cucu" );

  test.printNone.connect( &imp, &Do::printNone );
  test.printNone.connect( printNone );

  test.printNone( );

  test.printStr.disconnect( &imp, &Do::printStr );
  test.printStr( "cucu" );
  test.printStr.disconnect( printStr );
  test.printStr( "cucu" );

  

  return 0;
}
