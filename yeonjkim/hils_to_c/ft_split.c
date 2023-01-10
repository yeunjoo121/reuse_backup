#include "hils_pi.h"

static char             **ft_malloc_error(char **str)
{
        int     index;

        index = 0;
        while (!str[index])
        {
                free(str[index++]);
        }
        free(str);
        return (NULL);
}

static char             *ft_strndup(const char *s, int len)
{
        char    *res;
        int             index;

        res = NULL;
        index = 0;
        if (len == 0)
                return (NULL);
        if (!(res = (char*)malloc(sizeof(char) * (len + 1))))
        {
                return (NULL);
        }
        while (index < len)
        {
                res[index] = s[index];
                index++;
        }
        res[index] = '\0';
        return (res);
}

static char             *ft_makestr(const char *s1, char c, int *flag)
{
        char    *middle;
        int             index;

        *flag = 1;
        middle = NULL;
        index = 0;
        while (s1[index] != '\0')
        {
                if (s1[index] == c)
                {
                        middle = ft_strndup(s1, index);
                        if (!middle)
                                return (NULL);
                        return (middle);
                }
                else if (index == (int)(strlen(s1)) - 1)
                {
                        middle = ft_strndup(s1, index + 1);
                        if (!middle)
                                return (NULL);
                        return (middle);
                }
                index++;
        }
        return (NULL);
}

static int              ft_word_count(const char *s, char c)
{
        int             wc;
        int             index;

        wc = 0;
        index = 0;
        while (s[index] != '\0')
        {
                if ((index == 0 && s[0] != c) ||
                (s[index] == c && s[index + 1] != c && s[index + 1] != '\0'))
                {
                        wc++;
                }
                index++;
        }
        return (wc);
}

char                    **ft_split(const char *s, char c, int *len)
{
        char    **str;
        char    *middle;
        int             flag;
        int             index;
        int             strindex;

        strindex = 0;
        index = 0;
        if (!(str = (char**)malloc(sizeof(char*) * (ft_word_count(s, c) + 1))))
                return (NULL);
        while (s[index] != '\0')
        {
                flag = 0;
                if (index == 0 && s[0] != c)
                        middle = ft_makestr(&s[index], c, &flag);
                else if ((s[index] == c && s[index + 1] != c && s[index + 1] != '\0'))
                        middle = ft_makestr(&s[index + 1], c, &flag);
                if (flag == 1 && middle == NULL)
                        return (ft_malloc_error(str));
                else if (flag == 1)
                        str[strindex++] = middle;
                index++;
        }
        str[strindex] = 0;
        *len = strindex;
        return (str);
}
