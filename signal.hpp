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
};

template< typename FIRST = void, typename... ARGS > class Signal : public SignalBase {
public:
  using SignalBase::size_type;

  void connect( void( *target )( FIRST, ARGS... ) ) {
    m_vector.emplace_back( reinterpret_cast< void * >( target ) );
  }
  template< typename CLASS > void connect( CLASS *self, void ( CLASS::*method )( FIRST, ARGS... ) ) {
    auto lambda = []( Target &target, FIRST f, ARGS... args ) {
      auto tself = reinterpret_cast< CLASS * >( target.self );
      auto tmethod = reinterpret_cast< void ( CLASS::* )( FIRST, ARGS... ) >( target.method );

      ( tself->*tmethod )( std::forward< FIRST >( f ), std::forward< ARGS >( args )... );
    };

    m_vector.emplace_back( reinterpret_cast< void * >( *lambda ), reinterpret_cast< void * >( self ), reinterpret_cast< void ( CLASS::* )( ) >( method ) );
  }
  void operator()( FIRST &&f, ARGS&&... args ) {
    for( auto &iter: m_vector ) {
      if( iter.self ) {
        reinterpret_cast< void ( * )( Target &, FIRST, ARGS... ) >( iter.callback )( iter, std::forward< FIRST >( f ), std::forward< ARGS >( args )... );
      } else {
        reinterpret_cast< void( * )( FIRST, ARGS... ) >( iter.callback )( std::forward<FIRST>( f ), std::forward< ARGS >( args )... );
      }
    }
  }
};

template< > class Signal< void > : public SignalBase {
public:
  using SignalBase::size_type;

  void connect( void( *target )( ) ) {
    m_vector.emplace_back( reinterpret_cast< void * >( target ) );
  }
  template< typename CLASS > void connect( CLASS *self, void ( CLASS::*method )( ) ) {
    auto lambda = []( Target &target ) {
      auto tself = reinterpret_cast< CLASS * >( target.self );
      auto tmethod = reinterpret_cast< void ( CLASS::* )( ) >( target.method );

      ( tself->*tmethod )( );
    };

    m_vector.emplace_back( reinterpret_cast< void * >( *lambda ), reinterpret_cast< void * >( self ), reinterpret_cast< void ( CLASS::* )( ) >( method ) );    
  }
  void operator()( ) {
    for( auto &iter: m_vector ) {
      if( iter.self ) {
        reinterpret_cast< void ( * )( Target & ) >( iter.callback )( iter );
      } else {
        reinterpret_cast< void( * )( ) >( iter.callback )( );
      }
    }
  }
};

#endif
