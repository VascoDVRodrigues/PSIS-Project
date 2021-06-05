#define NO_ERROR 0
#define WRONG_SECRET -1
#define WRONG_GROUP -2
#define ERROR_CONNECTION -3
#define TIMED_OUT -4
#define FULL_SERVER -5
#define GROUP_DELETED -6
#define NO_KEY -7
#define SERVER_ERROR -8

/*
 * Function: establish_connection
 * ----------------------------
 *   Connects the app to the local server
 *
 *   group_id: id of the group trying to connect (MAX SIZE 1024)
 *   secret: secret of the group trying to connect (MAX SIZE 1024)
 *
 *   returns:NO_ERROR: if the connection was successfully made
 * 		     WRONG_SECRET: the group exists but the secret isn't correct
 * 			 WRONG_GROUP: the group doesn't exist
 * 			 ERROR_CONNECTION: error in the connection to the server
 * 			 TIMED_OUT: timed out error
 * 		     FULL_SERVER: Full server
 */
int establish_connection(char *group_id, char *secret);

/*
 * Function: put_value
 * ----------------------------
 *   Given a key and a value, it stores those values on the connected group
 *
 *   key: a key to identify the stored value (MAX SIZE 128)
 *   value: the value to store (NO MAX SIZE)
 *
 *   returns:NO_ERROR: the value was successfully stored on the local server
 * 		     SERVER_ERROR: there was an error saving the value on the local server
 * 			 GROUP_DELETED: the group doesnt exist. Connection will be automatically closed
 * 			 ERROR_CONNECTION: there was an error comunicating with the server
 *
 */
int put_value(char *key, char *value);

/*
 * Function: get_value
 * ----------------------------
 *   Given a key and a pointer to a variable, it stores the value of said key on the connected group to the variable pointed by the pointer
 *
 *   key: a key to identify the value to retrieve
 *   value: the pointer to the variable in which the value of the key will be stored
 *
 *   returns:NO_ERROR: the value was successfully retrieved from the local server
 * 		     SERVER_ERROR: there was an error retrieving the value from the local server
 * 			 GROUP_DELETED: the group doesnt exist. Connection will be automatically closed
 *			 ERROR_CONNECTION: there was an error comunicating with the server
 *			 NO_KEY: the key doesnt exist in that group
 */
int get_value(char *key, char **value);

/*
 * Function: delete_value
 * ----------------------------
 *   Given a key it deletes the value of said key on the connected group
 *
 *   key: a key to identify the value to delete
 *
 *   returns: NO_ERROR: the value was successfully retrieved from the local server
 * 		     SERVER_ERROR: there was an error deleting the value from the local server
 *			 GROUP_DELETED: the group doesnt exist. Connection will be automatically closed
 *			 ERROR_CONNECTION: there was an error comunicating with the server
 *
 */
int delete_value(char *key);

/*
 * Function: register_callback
 * ----------------------------
 *   Given a key it registeres a function to be executed whenever the value of that key is altered
 *
 *   key: a key to identify the value whose update triggers the callback
 *   (*callback_function)(char*): pointer to function that MUST receive a char* and return void
 *
 *   returns: NO_ERROR: the callback was successfully implemented
 *			 SERVER_ERROR: error registering the callback
 *
 */
int register_callback(char *key, void (*callback_function)(char *));

/*
 * Function: close_connection
 * ----------------------------
 *   Closes the connection between a client and the local server
 *
 *   returns:NO_ERROR: the connection was successfully closed
 *			 ERROR_CONNECTION: there was an error comunicating with the server
 *
 */
int close_connection();