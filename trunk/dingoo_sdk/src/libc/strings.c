#include <strings.h>

int ffs(int i) {
	int j;
	for (j = 0; j < 32; j++) {
		if (((1 << j) & i) != 0)
			return j + 1;
	}

	return 0;
}
