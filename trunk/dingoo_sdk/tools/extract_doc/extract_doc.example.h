/**
=filename.h=
Generic header description here. Demonstration on how to write documentation
for use in Google Code wiki and the extract_doc.py tool.
*/

/**
==Globals==
*/
/**
===[ship] *my_ship===
Stores the master copy of a [ship].
*/
ship *my_ship;

/**
==Structures==
*/

/**
===my_thing===
Description of this struct.

Members:
  * *item_one*: an _int_ that counts something
  * *item_two*: a _char_* containing the name
  * *fluff*: an [another_thing]* for fluff
*/
typedef struct {
	int item_one;
	char *item_two;
	another_thing *fluff;
	double undocumented_var; /* this is an internal-only var and comment */
} my_thing;

/**
==Macros==
*/
/**
==_int_ doSomething( _int_ x, _int_ y )==
Description of this macro. Given an _int_ x and an _int_ y, returns 0 if they
are equal, non-zero if they are not.
*/
#define doSomething ( x && y )

/**
==BUTTON_ONE==
Represents the first button on the device for use with the various functions
in [control.h].
*/
#define BUTTON_ONE 1

/**
==Methods==
*/

/**
==_char_* strcpy( _char_* from, [my_thing]* to )==
Copies the given string to the given [my_thing]. Returns the new string held
within the given [my_thing].
*/
char *strcpy( char *from, my_thing *to );
