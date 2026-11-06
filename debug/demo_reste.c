/*
** Démo : pourquoi déplacer un pointeur ne "garde" pas le reste.
**
** Compile et lance (depuis la racine du repo) :
**     cc -Wall -Wextra -Werror debug/demo_reste.c -o demo && ./demo
**
** Ce fichier est indépendant de ton get_next_line : il simule les
** différentes méthodes et affiche le contenu du tableau à chaque étape.
** Dans l'affichage, \n et \0 sont écrits en clair pour qu'on les voie.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TAILLE 4

/* Affiche le tableau case par case : [a][\n][b]... */
static void	affiche(const char *titre, const char *tab, int taille)
{
	int	i;

	printf("    %-22s", titre);
	i = 0;
	while (i < taille)
	{
		if (tab[i] == '\n')
			printf("[\\n]");
		else if (tab[i] == '\0')
			printf("[\\0]");
		else
			printf("[%c ]", tab[i]);
		i++;
	}
	printf("\n");
}

/* Affiche une ligne en rendant le \n visible */
static void	affiche_ligne(const char *debut)
{
	printf("    -> ligne renvoyee : \"");
	while (*debut && *debut != '\n')
		putchar(*debut++);
	if (*debut == '\n')
		printf("\\n");
	printf("\"\n");
}

/* Crée un "fichier" en mémoire (un pipe) qui contient le texte donné */
static int	faux_fichier(const char *texte)
{
	int	p[2];

	if (pipe(p) < 0)
		return (-1);
	write(p[1], texte, strlen(texte));
	close(p[1]);
	return (p[0]);
}

/* ===================================================================== */
/*  DEMO 1 : modifier un paramètre ne change rien pour l'appelant        */
/* ===================================================================== */

static void	avance(char *tab_debut, char *buff)
{
	printf("    dans la fonction, au debut : buff pointe sur la case %ld\n",
		(long)(buff - tab_debut));
	buff = strchr(buff, '\n') + 1;
	printf("    dans la fonction, apres buff = strchr(buff, '\\n') + 1 :"
		" buff pointe sur la case %ld\n", (long)(buff - tab_debut));
}

static void	demo1(void)
{
	static char	tab[8] = "a\nb\nc\n";
	int			appel;

	printf("\n===== DEMO 1 : le pointeur avance... puis revient au debut "
		"=====\n\n");
	affiche("tableau static :", tab, 8);
	appel = 1;
	while (appel <= 3)
	{
		printf("\n  appel %d : on passe tab a la fonction\n", appel);
		avance(tab, tab);
		printf("    de retour dans l'appelant : tab pointe toujours sur "
			"la case 0\n");
		appel++;
	}
	printf("\n  => Le 'buff = ...' ne modifie qu'une COPIE locale de "
		"l'adresse.\n");
	printf("     A chaque appel on recommence a la case 0, et on retombe "
		"sur le MEME \\n.\n");
}

/* ===================================================================== */
/*  DEMO 2 : ta méthode (read à la fin) -> la même ligne en boucle       */
/* ===================================================================== */

static void	ta_methode(int fd, char *tab)
{
	char	*buff;
	long	n;

	buff = tab;
	if (strchr(buff, '\n'))
		buff = strchr(buff, '\n') + 1;
	printf("    on repart de la case %ld\n", (long)(buff - tab));
	if (*buff == '\0')
	{
		n = read(fd, tab, 7);
		if (n < 0)
			n = 0;
		tab[n] = '\0';
		buff = tab;
		printf("    buffer vide -> read() de %ld octets\n", n);
	}
	affiche("tableau static :", tab, 8);
	affiche_ligne(buff);
}

static void	demo2(void)
{
	static char	tab[8];
	int			fd;
	int			appel;

	printf("\n===== DEMO 2 : ta methode, fichier \"a\\nb\\nc\\n\" lu d'un "
		"coup =====\n");
	fd = faux_fichier("a\nb\nc\n");
	appel = 1;
	while (appel <= 4)
	{
		printf("\n  appel %d :\n", appel);
		ta_methode(fd, tab);
		appel++;
	}
	close(fd);
	printf("\n  => On attendait a, b, c. On obtient a, b, b, b... "
		"pour toujours.\n");
}

/* ===================================================================== */
/*  DEMO 3 : read() au milieu du tableau -> débordement                  */
/* ===================================================================== */

struct s_memoire
{
	char	tab[TAILLE + 1];
	char	voisin[8];
};

static void	demo3(void)
{
	static struct s_memoire	m = {"", "COUCOU"};
	char					*buff;
	int						fd;
	long					n;

	printf("\n===== DEMO 3 : read() au milieu du tableau, BUFFER_SIZE=%d "
		"=====\n\n", TAILLE);
	printf("  En memoire, juste apres ton tableau, il y a une autre "
		"variable \"voisin\".\n\n");
	fd = faux_fichier("ab\ncdefgh\n");
	n = read(fd, m.tab, TAILLE);
	m.tab[n] = '\0';
	printf("  1er read() de %d octets dans tab :\n", TAILLE);
	affiche("tab :", m.tab, TAILLE + 1);
	affiche("voisin :", m.voisin, 8);
	buff = strchr(m.tab, '\n') + 1;
	printf("\n  buff = strchr(tab, '\\n') + 1  -> buff pointe sur la case "
		"%ld\n", (long)(buff - m.tab));
	printf("  2e read(fd, buff, %d) : on ecrit %d octets a partir de la "
		"case %ld...\n", TAILLE, TAILLE, (long)(buff - m.tab));
	n = read(fd, buff, TAILLE);
	buff[n] = '\0';
	affiche("tab :", m.tab, TAILLE + 1);
	affiche("voisin :", m.voisin, 8);
	close(fd);
	printf("\n  => Le 'c' du reste a ete ecrase, et read() a deborde dans "
		"voisin.\n");
	printf("     Dans ton vrai code, ca peut planter ou corrompre "
		"d'autres variables.\n");
}

/* ===================================================================== */
/*  DEMO 4 : la bonne méthode -> on recopie le reste au début            */
/* ===================================================================== */

static void	decale_reste(char *tab)
{
	char	*nl;
	int		j;

	nl = strchr(tab, '\n');
	if (!nl)
	{
		tab[0] = '\0';
		return ;
	}
	j = 0;
	while (nl[1 + j])
	{
		tab[j] = nl[1 + j];
		j++;
	}
	tab[j] = '\0';
}

static void	demo4(void)
{
	static char	tab[8];
	int			fd;
	int			appel;
	long		n;

	printf("\n===== DEMO 4 : la bonne methode (on decale le reste au "
		"debut) =====\n");
	fd = faux_fichier("a\nb\nc\n");
	appel = 1;
	while (appel <= 4)
	{
		printf("\n  appel %d :\n", appel);
		affiche("tableau au debut :", tab, 8);
		if (tab[0] == '\0')
		{
			n = read(fd, tab, 7);
			if (n < 0)
				n = 0;
			tab[n] = '\0';
			printf("    buffer vide -> read() de %ld octets\n", n);
			affiche("apres read :", tab, 8);
		}
		if (tab[0] == '\0')
			printf("    -> plus rien a lire : renvoie NULL\n");
		else
		{
			affiche_ligne(tab);
			decale_reste(tab);
			affiche("apres decalage :", tab, 8);
		}
		appel++;
	}
	close(fd);
	printf("\n  => Le debut du tableau contient TOUJOURS ce qui reste a "
		"traiter.\n");
	printf("     Pas besoin de se souvenir d'une position : on repart "
		"de la case 0.\n");
	printf("     (Les cases apres le premier \\0 sont de vieux restes : "
		"toutes les fonctions\n      s'arretent au \\0, donc elles sont "
		"ignorees.)\n");
}

int	main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	demo1();
	demo2();
	demo3();
	demo4();
	printf("\n");
	return (0);
}
