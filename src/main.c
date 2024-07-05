/**
 *
 ************************************************************************* **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <openssl/ssl3.h>

#define BUFSIZE 4096

/**
 * Arguments descriptor
 */
typedef struct
{
    char *progname;
    char *refpath;
    char *prunepath;
    int do_hash;
} ArgsStruct;

char prunepath[4096];
char refpath[4096];

/** Compare binary files contents. The file names are in the global variables
 * @return Returns 0 if the files are identical.
 *************************************************************************** */
int CompareFiles()
{
    FILE *fref = fopen(refpath, "rb");
    FILE *fprune = fopen(prunepath, "rb");
    static unsigned char prunebuf[BUFSIZE], refbuf[BUFSIZE];
    static unsigned char md[SHA256_DIGEST_LENGTH];
    int prunelen;
    int reflen;
    SHA256_CTX ctx;

    // Check input files
    if (NULL == fref)
    {
        printf("!!! Unable to open %s\n", refpath);
    }
    if (NULL == fprune)
    {
        printf("!!! Unable to open %s\n", prunepath);
    }

    SHA256_Init(&ctx);
    // Read and compare
    do
    {
        prunelen = fread(prunebuf, 1, BUFSIZE, fprune);
        reflen = fread(refbuf, 1, BUFSIZE, fref);
        if (prunelen != reflen)
        {
            printf("??? %s: %d != %d\n", prunepath, prunelen, reflen);
            return -1;
        }
        if (!memcmp(prunebuf, refbuf, prunelen))
        {
            return -1;
        }
        if (reflen < BUFSIZE)
        {
            memset(refbuf + reflen, 0, BUFSIZE - reflen);
        }
        SHA256_Update(&ctx, refbuf, reflen);
    } while ((prunelen == reflen) && (prunelen > 0));
    SHA256_Final(md, &ctx);

    // Clean up and exit
    fclose(fref);
    fclose(fprune);
    return 0;
}

/**
 *
 *************************************************************************** */
int ParseTree()
{
    DIR *prunedir = opendir(prunepath);
    struct dirent *pruneent;
    char *pruneend = prunepath + strlen(prunepath) - 1;
    char *refend = refpath + strlen(refpath) - 1;
    struct stat prunestat;

    if ((*pruneend != '/') && (*pruneend != '\\'))
    {
        *(++pruneend) = '/';
    }
    *(++pruneend) = 0;
    if ((*refend != '/') && (*refend != '\\'))
    {
        *(++refend) = '/';
    }
    *(++refend) = 0;

    while (NULL != (pruneent = readdir(prunedir)))
    {
        char *name = pruneent->d_name;
        if (name[0] == '.')
        {
            if (name[1] == 0)
            {
                continue;
            }
            else if ((name[1] == '.') && (name[2] == 0))
            {
                continue;
            }
        }
        strcpy(pruneend, name);
        strcpy(refend, name);
        if (0 == stat(prunepath, &prunestat))
        {
            if (S_ISREG(prunestat.st_mode))
            {
                printf("  * %s --> %d\n", prunepath, CompareFiles());
            }
            else if (S_ISDIR(prunestat.st_mode))
            {
                printf("%s is a dir\n", prunepath);
                ParseTree();
            }
        }
    }

    *(--refend) = 0;
    *(--pruneend) = 0;
    return 0;
}

/*
 *
 *************************************************************************** */
int ParseArgs(ArgsStruct *args, int ac, char *av[])
{
    args->progname = *av++;
    while (--ac > 0)
    {
        char *arg = *av++;

        if (*arg == '-')
        {
            switch (arg[1])
            {
            case 'p':
                ac--;
                args->prunepath = *(++av);
                break;

            case 'r':
                ac--;
                args->refpath = *(++av);
                break;

            case 'x':
                args->do_hash = 1;
                break;

            default:
                break;
            }
        }
        else
        {
            if (NULL == args->refpath)
            {
                args->refpath = arg;
            }
            else if (NULL == args->prunepath)
            {
                args->prunepath = arg;
            }
        }
    }
    printf("progname %s\n", args->progname);
    printf("refpath %s\n", args->refpath);
    printf("prunepath %s\n", args->prunepath);
    return 0;
}

/*
 *
 *************************************************************************** */
int main(int ac, char *av[])
{
    int ret = 0;
    ArgsStruct args = {
        .progname = NULL,
        .refpath = NULL,
        .prunepath = NULL,
        .do_hash = 0};

    ParseArgs(&args, ac, av);
    strcpy(prunepath, args.prunepath);
    strcpy(refpath, args.refpath);
    ParseTree();

    return ret;
}
