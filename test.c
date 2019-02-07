#include <libNUMA.h>
#include <stdio.h>

static void
print_set (size_t len, const void *p)
{
  const unsigned long int *tp = p;
  while (len > 0)
    {
      unsigned long int v = *tp++;
      unsigned long int mask = 1ul;
      while (mask != 0)
	{
	  putchar_unlocked ('0' + ((v & mask) != 0));
	  mask <<= 1;
	}

      len -= sizeof (*tp);
    }
}

int
main (void)
{
  cpu_set_t s;

  printf ("NUMA_cpu_system_count = %d\n", NUMA_cpu_system_count ());

  printf ("NUMA_cpu_system_mask = ");
  if (NUMA_cpu_system_mask (sizeof (s), &s) < 0)
    puts ("<N/A>");
  else
    {
      print_set (sizeof (s), &s);
      putchar_unlocked ('\n');
    }

  printf ("NUMA_cpu_self_count = %d\n", NUMA_cpu_self_count ());

  printf ("NUMA_cpu_self_mask = ");
  if (NUMA_cpu_self_mask (sizeof (s), &s) < 0)
    puts ("<N/A>");
  else
    {
      print_set (sizeof (s), &s);
      putchar_unlocked ('\n');
    }

  /* Find one valid CPU.  */
  puts ("NUMA_cpu_system_mask");
  if (NUMA_cpu_system_mask (sizeof (s), &s) >= 0 && CPU_COUNT (&s) != 0)
    {
      size_t idx = 0;
      while (! CPU_ISSET (idx, &s))
	++idx;

      CPU_ZERO (&s);
      CPU_SET (idx, &s);

      size_t level = 0;
      while (1)
	{
	  cpu_set_t s2;
	  ssize_t n;
	  n = NUMA_cpu_level_mask (sizeof (s2), &s2, sizeof (s), &s, level);
	  if (n < 0)
	    {
	      printf ("NUMA_cpu_level_mask for level %zu failed\n", level);
	      break;
	    }
	  if (n < level)
	    break;

	  printf ("level %zu: ", level);
	  print_set (sizeof (s2), &s2);
	  putchar_unlocked ('\n');

	  ++level;
	}
    }

  return 0;
}
