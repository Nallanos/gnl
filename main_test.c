#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "get_next_line.h"

/*
** Modes :
**   ./gnl fichier      lit le fichier et affiche toutes les lignes
**   ./gnl              lit l'entrée standard
**   ./gnl --invalid    teste des fd invalides (doit renvoyer NULL)
*/

static int	test_invalid(void)
{
	char	*line;
	int		ko;

	ko = 0;
	line = get_next_line(-1);
	if (line != NULL)
	{
		fprintf(stderr, "fd -1 : NULL attendu\n");
		free(line);
		ko = 1;
	}
	line = get_next_line(1000);
	if (line != NULL)
	{
		fprintf(stderr, "fd 1000 (non ouvert) : NULL attendu\n");
		free(line);
		ko = 1;
	}
	return (ko);
}

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc > 1 && strcmp(argv[1], "--invalid") == 0)
		return (test_invalid());
	fd = 0;
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (2);
	line = get_next_line(fd);
	while (line != NULL)
	{
		fputs(line, stdout);
		free(line);
		line = get_next_line(fd);
	}
	line = get_next_line(fd);
	if (line != NULL)
	{
		fprintf(stderr, "appel apres la fin : NULL attendu\n");
		free(line);
		return (3);
	}
	if (fd != 0)
		close(fd);
	return (0);
}
