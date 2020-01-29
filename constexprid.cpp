#include <cstdint>
#include <iostream>

template< typename T > class ConstexprId {
  static constexpr void dummy( ) { }

public:
  static constexpr const void *id = reinterpret_cast< const void * >( dummy );
};

template< typename > class ConstexprFnId{ };

template< typename RET, typename... ARGS > class ConstexprFnId< RET( ARGS... ) > {
  static constexpr void dummy( ) { }

public:
  static constexpr const void *id = reinterpret_cast< const void * >( dummy );
};

template< typename RET, typename CLASS, typename... ARGS > class ConstexprFnId< RET (CLASS::*)( ARGS... ) > {
  static void dummy( ) { }

public:
  static constexpr const void *id = reinterpret_cast< const void * >( dummy );
};

template< typename RET, typename CLASS, typename... ARGS > class ConstexprFnId< RET (CLASS::*)( ARGS... ) const > {
  static void dummy( ) { }

public:
  static constexpr const void *id = reinterpret_cast< const void * >( dummy );
};

template< typename RET, typename CLASS, typename... ARGS > class ConstexprFnId< RET (CLASS::*)( ARGS... ) volatile > {
  static void dummy( ) { }

public:
  static constexpr const void *id = reinterpret_cast< const void * >( dummy );
};

struct Test {
  static void algo( );
  void algo2( );
};

void TestFn( ) { }

int main( ) {
  std::cout << "Test id: " << ConstexprId< Test >::id;
  std::cout << "\nTestFn id: " << ConstexprFnId< decltype( TestFn ) >::id;
  std::cout << "\nstatic Test::algo id: " << ConstexprFnId< decltype( Test::algo ) >::id;
  std::cout << "\nTest::algo2 id: " << ConstexprFnId< decltype( &Test::algo2 ) >::id << '\n';

  return 0;
}
