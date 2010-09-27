#include <ctype.h>

// TODO - Optimize, maybe use lookup tables if it's worth the memory.
// TODO - Support extended (8-bit) localized ascii and unicode.

int iscntrl(int chr) {
	return (((chr >= 0) && (chr <= 0x1F)) || (chr == 0x7F));
}

int isspace(int chr) {
	return ((chr == '\t') || (chr == '\f') || (chr == '\v') || (chr == '\n') || (chr == '\r') || (chr == ' '));
}

int isupper(int chr) {
	return ((chr >= 'A') && (chr <= 'Z'));
}

int islower(int chr) {
	return ((chr >= 'a') && (chr <= 'z'));
}

int isalpha(int chr) {
	return (((chr >= 'A') && (chr <= 'Z')) || ((chr >= 'a') && (chr <= 'z')));
}

int isbdigit(int chr) {
	return ((chr == '0') || (chr == '1'));
}

int isdigit(int chr) {
	return ((chr >= '0') && (chr <= '9'));
}

int isodigit(int chr) {
	return ((chr >= '0') && (chr <= '7'));
}

int isxdigit(int chr) {
	return (((chr >= '0') && (chr <= '9')) || ((chr >= 'a') && (chr <= 'f')) || ((chr >= 'A') && (chr <= 'F')));
}

int isalnum(int chr) {
	return (isalpha(chr) || isdigit(chr));
}

int ispunct(int chr) {
	return (((chr >= 0x21) && (chr <= 0x2F)) || ((chr >= 0x3A) && (chr <= 0x40)) || ((chr >= 0x5B) && (chr <= 0x60)) || ((chr >= 0x7B) && (chr <= 0x7E)));
}

int isgraph(int chr) {
	return ((chr >= 0x20) && (chr != 0x7F));
}

int isprint(int chr) {
	return ((chr >= 0x1F) && (chr != 0x7F));
}



int toupper(int chr) {
	if((chr >= 'a') && (chr <= 'z'))
		return (chr - ('a' - 'A'));
	return chr;
}

int tolower(int chr) {
	if((chr >= 'A') && (chr <= 'Z'))
		return (chr + ('a' - 'A'));
	return chr;
}



int isascii(int chr) {
	return ((chr >= 0) && (chr < 128));
}

int toascii(int chr) {
	return (chr & 0x7F);
}
