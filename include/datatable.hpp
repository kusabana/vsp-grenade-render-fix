#include <vector>

constexpr int PROP_TYPE_DT = 6;

class data_table_t;

struct prop_t {
  void *vtable;
  void *matching_recv_prop;
  int type;
  int bits;
  float low_value;
  float high_value;

  prop_t *array_prop;
  int ( *array_len_proxy_fn )( const void *, int );

  int elements;
  int element_stride;

  const char *exclude_dt_name;
  const char *parent_array_prop_name;

  const char *name;
  float high_low_mul;

  int flags;

  void ( *proxy_fn )(
      const void *, const void *, const void *, void *, int, int );
  void *( *send_table_proxy_fn )(
      const prop_t *, const void *, const void *, void *, int );

  data_table_t *table;
  int offset;
  const void *extra_data;
};

struct data_table_t {
  prop_t *props;
  int prop_count;
  const char *table_name;
};

class prop_flat_hierarchy {
public:
  using iterator = typename std::vector< prop_t * >::iterator;
  using reverse_iterator = typename std::vector< prop_t * >::reverse_iterator;

  explicit prop_flat_hierarchy( data_table_t *root ) { flatten_props( root ); }

  iterator begin( ) { return flattened_props_.begin( ); }
  iterator end( ) { return flattened_props_.end( ); }

  reverse_iterator rbegin( ) { return flattened_props_.rbegin( ); }
  reverse_iterator rend( ) { return flattened_props_.rend( ); }

private:
  std::vector< prop_t * > flattened_props_;

  void flatten_props( data_table_t *table ) {
    if ( !table )
      return;

    for ( int i = 0; i < table->prop_count; ++i ) {
      prop_t *prop = &table->props[ i ];
      flattened_props_.push_back( prop );

      if ( prop->type == PROP_TYPE_DT && prop->table ) {
        flatten_props( prop->table );
      }
    }
  }
};