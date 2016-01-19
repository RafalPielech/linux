#include <stdio.h>
#include <stdlib.h>

struct s_type {
	int i;
	unsigned u;
};

int main() {
	struct s_type *st;
	st = malloc(10 * sizeof(struct s_type));
	if(st)
	{
		(st+5)->i = 3;
		st[5].u = 6;
		printf("%d %u\n", st[5].i*3, (st+5)->u*3);
		free(st);
	}
	return 0;
}

