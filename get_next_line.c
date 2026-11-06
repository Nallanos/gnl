/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abenameu <abenameu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 10:08:37 by abenameu          #+#    #+#             */
/*   Updated: 2026/11/06 12:40:27 by abenameu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	ft_strswap(char **line, char buff[])
{
	char	*temp;
	temp = malloc(ft_strlen(*line) + ft_strlen(buff) + 1);
	if (!temp)
		return ;
	temp[ft_strlen(*line) + ft_strlen(buff)] = '\0';
	ft_strcat(temp, *line);
	ft_strcat(temp, buff);
	free(*line);
	*line = temp;
}

int	fill_line(char **line, char buff[])
{
	int	i;

	i = 0;
	ft_strswap(line, buff);
	while (buff[i] && buff[i] != '\n')
	{
		(*line)[i] = buff[i];
		i++;
	}
	if (buff[i] == '\n')
	{
		(*line)[i] = buff[i];
		i++;
	}
	return (i);
}

void	rotate_buff(char buff[], int len)
{
	
}

char	*create_line(int fd, char buff[])
{
	int		byte;
	char	*line;
	char	*temp;

	line = NULL;
	temp = NULL;
	byte = 1;
	while (byte > 0)
	{
		byte = read(fd, buff, BUFFER_SIZE);
		fill_line(&line, buff);

	}
	if (byte < 0)
	{
		if (line)
			free(line);
		if (temp)
			free(temp);
		return (NULL);
	}
	return (line);
}
char	*get_next_line(int fd)
{
	static char	buff[BUFFER_SIZE + 1];
	char		*line;

	if (fd < 0)
		return (NULL);
	if (BUFFER_SIZE == 10000)
		buff[BUFFER_SIZE] = '\0';
	line = create_line(fd, buff);
	return (line);
}
