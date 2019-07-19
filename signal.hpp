#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <vector>

class SignalBase {
protected:
  struct Target {
    void *callback;
    void *self;
    void ( Target::*method )( );

    Target( const Target & ) = default;
    Target( void *cb  ) : callback( cb ), self( nullptr ), method( nullptr ) { }
    template< typename T > Target( void *cb, void *s, void ( T::*m )( ) ) : callback( cb ), self( s ), method( reinterpret_cast< void ( Target::* )( ) >( m ) ) { }
  };

  std::vector< Target > m_vector;

  SignalBase( ) = default;

public:
  using size_type = decltype( m_vector.size( ) );

  bool empty( ) const { return m_vector.empty( ); }
  size_type size( ) const { return m_vector.size( ); }
  void clear( ) { m_vector.clear( ); }

  template< typename... ARGS > void connect( void ( *target )( ARGS... ) ) {
        m_vector.emplace_back( reinterpret_cast< void * >( target ) );
  }
  template< typename CLASS, typename... ARGS > void connect( CLASS *self, void ( CLASS::*method )( ARGS... ) ) {
    auto lambda = []( Target &target, ARGS... args ) {
      auto tself = reinterpret_cast< CLASS * >( target.self );
      auto tmethod = reinterpret_cast< void ( CLASS::* )( ARGS... ) >( target.method );

      ( tself->*tmethod )( std::forward< ARGS >( args )... );
    };

    m_vector.emplace_back( reinterpret_cast< void * >( *lambda ), reinterpret_cast< void * >( self ), reinterpret_cast< void ( CLASS::* )( ) >( method ) );
  }
  template< typename... ARGS > void operator()( ARGS... args ) {
    for( auto &iter: m_vector ) {
      if( iter.self ) {
        reinterpret_cast< void ( * )( Target &, ARGS... ) >( iter.callback )( iter, std::forward< ARGS >( args )... );
      } else {
        reinterpret_cast< void( * )( ARGS... ) >( iter.callback )( std::forward< ARGS >( args )... );
      }
    }    
  }
};

template< typename FIRST = void, typename... ARGS > class Signal : public SignalBase {
public:
  using SignalBase::size_type;
};

template< > class Signal< void > : public SignalBase {
public:
  using SignalBase::size_type;
};

#endif
