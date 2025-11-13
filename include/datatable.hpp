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
