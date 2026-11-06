#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;
	int		i;

	fd = open("test.txt", O_RDONLY);
	i = 1;
	while (i <= 4)
	{
		line = get_next_line(fd);
		printf("appel %d : [%s]\n", i, line);
		free(line);
		i++;
	}
	return (0);
}
