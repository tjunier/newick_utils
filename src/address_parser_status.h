

enum address_parser_status_type {
	ADDRESS_PARSER_OK,
	ADDRESS_PARSER_PARSE_ERROR,
	ADDRESS_PARSER_MALLOC_ERROR
	};

/* This variable is set by the address parser prior to returning. */
enum address_parser_status_type address_parser_status;
