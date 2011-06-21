
/** \file */

/** These constants are used to signal the status of the address parser. */

enum address_parser_status_type {
	ADDRESS_PARSER_OK,	/*!< parsing ok */
	ADDRESS_PARSER_PARSE_ERROR,	/*!< malloc() failed */
	ADDRESS_PARSER_MALLOC_ERROR	/*!< syntax error */
	};

/** This variable is set by the address parser prior to returning.  The parser
 * returns either \c NULL or the top-level enode of the address, so we need to
 * use an extern variable to convey its status. */

enum address_parser_status_type address_parser_status;
