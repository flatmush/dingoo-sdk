#include "locale.h"

// TODO - Offer a selection of locales.
struct lconv _locale = {
	".", ",", "\3",
	"GBP", "£",
	".", ",", "\3",
	"", "-",
	2, 2,
	1, 0, 1, 0, 1, 1,
	1, 0, 1, 0, 1, 1
	};



struct lconv* localeconv() {
	return &_locale;
}

char* setlocale(int category, const char* locale) {
	return NULL;
}
