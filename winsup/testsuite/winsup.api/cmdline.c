/* This file is part of Cygwin.
 *
 * Tests for cygwin_cmdline_() APIs.
 */

#include "../libltp/include/test.h"
#include <sys/cygwin.h>

const char *TCID = "cmdline";    	/* Test program identifier. */
int TST_TOTAL = 1;              /* Total number of test cases. */
extern int Tst_count;           /* Test Case counter for tst_* routines */

int
argvcmp (char *a[], char *b[])
{
  size_t i = 0;
  int res = 0;

  // content same?
  for (; a[i] && b[i]; i++)
    if (res = strcmp (a[i], b[i]))
      return res;

  // length same?
  return a[i] || b[i];
}

char *
stringify_argv (char *a[])
{
  size_t retsize = 1024;
  char *ret = malloc (retsize);
  size_t retpos = 0;

  for (size_t i = 0; a[i]; i++)
    {
      size_t len = strlen (a[i]);
      while (retsize < retpos + len)
        {
          retsize *= 2;
          ret = realloc (ret, retsize);
        }
      strcpy (ret + retpos, a[i]);
      retpos += len;
      ret[retpos++] = ',';
    }

  ret[retpos] = '\0';
  return ret;
}

#define PASS(...) tst_resm (TPASS, name, ## __VA_ARGS__)
#define FAIL(...) tst_resm (TFAIL, name_bad, ## __VA_ARGS__)

void
assert_argv_eq (const char* name, char *a[], char *b[])
{
  if (!argvcmp (a, b))
    {
      PASS (name);
      return;
    }

  const char *fault = " (expected [%s], got [%s])";
  char *name_bad = malloc (strlen (name) + strlen (fault) + 1);
  strcpy (name_bad, name);
  strcat (name_bad, fault);
  char *sa = stringify_argv (a), *sb = stringify_argv (b);
  FAIL (sa, sb);
  free (sa);
  free (sb);
}

int
parse_test (void)
{
  char *subject;
  char **av;

  tst_resm (TINFO, "Running %s", __FUNCTION__);

  subject = strdup ("a \\\\\\ \"\\a\\\\\"");
  char *av_exp1[4] = {"a", "\\\\\\", "\\a\\", NULL};
  cygwin_cmdline_parse (subject, &av, NULL, 0, 0);
  assert_argv_eq ("Backslashes rules", av_exp1, av);
  free (subject);

  subject = strdup ("a\"b\"c d\"\"e @\"f h \"\" i j\"");
  char *av_exp2[4] = {"abc", "d\"e", "@f h \" i j", NULL};
  cygwin_cmdline_parse (subject, &av, NULL, 0, 0);
  assert_argv_eq ("Mixed quoting environment", av_exp2, av);
  free (subject);
}

int
main (int argc, char *argv[])
{
  parse_test ();
  tst_exit ();
  /* NOTREACHED */
  return 0;
}
