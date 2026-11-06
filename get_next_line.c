/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abenameu <abenameu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 10:08:37 by abenameu          #+#    #+#             */
/*   Updated: 2026/11/06 12:01:43 by abenameu         ###   ########.fr       */
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

char	*create_line(int fd, char buff[])
{
	int		byte;
	char	*line;
	char	*temp;

	if (ft_strchr(buff, '\n'))
		buff = ft_strchr(buff, '\n');
	byte = 1;
	temp = NULL;
	while (byte > 0 && ft_strchr(buff, '\n'))
	{
		byte =  read(fd, buff, BUFFER_SIZE);
		if (!line)
			line = ft_strdup(buff);
		else
			ft_strswap(&line, buff);
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
