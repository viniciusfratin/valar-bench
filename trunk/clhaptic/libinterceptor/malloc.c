
/* Example of a library interposer: interpose on 
 * malloc().
 * Source Code: malloc_interposer.c
 * Build and use this interposer as following:
 * cc -o malloc_interposer.so -G -Kpic malloc_interposer.c
 * setenv LD_PRELOAD $cwd/malloc_interposer.so
 * run the app
 * unsetenv LD_PRELOAD
 */

#include <stdio.h>
#include <dlfcn.h>

void *malloc(size_t size)
{
  static void * (*func)();

  if(!func)
    func = (void *(*)()) dlsym(RTLD_NEXT, "malloc");
  printf("malloc(%d) is called\n", size);     
  return(func(size));
}


/* Library interposer to collect malloc/calloc/realloc 
 * statistics
 * and produce a histogram of their use.
 * Source Code: malloc_hist.c
 * cc -o malloc_hist.so -G -Kpic malloc_hist.c
 * setenv LD_PRELOAD $cwd/malloc_hist.so
 * run the application
 * unsetenv LD_PRELOAD
 *
 * The results will be in 
 * /tmp/malloc_histogram.<prog_name>.<pid>
 * for each process invoked by current application.
 */
#include <dlfcn.h>
#include <memory.h>
#include <assert.h>
#include <thread.h>
#include <stdio.h>
#include <procfs.h>
#include <fcntl.h>

typedef struct data {
  int histogram[32];
  char * caller;
} data_t;

data_t mdata = { 
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0, 
  "malloc"};

data_t cdata = { 
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0, 
  "calloc"};

data_t rdata = { 
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0, 
  "realloc"};

static FILE* output;
static int pid;
static char prog_name[32];
static char path[64];

static void done()
{
  fprintf(output, "prog_name=%s\n", prog_name);
  print_data(&mdata);
  print_data(&cdata);
  print_data(&rdata);
}

static int print_data(data_t * ptr)
{
  int i;

  fprintf(output, "******** %s **********\n", 
    ptr->caller);
  for(i=0;i<32;i++)
      if(i < 10 || ptr->histogram[i])
    fprintf(output, "%10u\t%10d\n", 1<<i, 
    ptr->histogram[i]);
}

exit(int status)
{
  char procbuf[32];
  psinfo_t psbuf;
  int fd;

  /* Get current executable's name using proc(4) 
     interface */
  pid = (int)getpid();
  (void)sprintf(procbuf, "/proc/%ld/psinfo", (long)pid);
  if ((fd = open(procbuf, O_RDONLY)) != -1)
  {
    if (read(fd, &psbuf, sizeof(psbuf)) == sizeof(psbuf))
      sprintf(prog_name, "%s.%d", psbuf.pr_fname, pid);
    else
      sprintf(prog_name, "%s.%d", "unknown", pid);
  }
  else
    sprintf(prog_name, "%s.%d", "unknown", pid);
  sprintf(path, "%s%s", "/tmp/malloc_histogram.", 
  prog_name);

  /* Open the file here since
     the shell closes all file descriptors 
     before calling exit() */
  output = fopen(path, "w");
  if(output)
      done();
  (*((void (*)())dlsym(RTLD_NEXT, "exit")))(status);
}

static int bump_counter(data_t * ptr, int size)
{
  static mutex_t lock;
  int size_orig;
  int i = 0;

  size_orig = size;
  while(size /= 2)
      i++;
  if(1<<i < size_orig)
      i++;

  /* protect histogram data if application is 
     multithreaded */  
  mutex_lock(&lock);
  ptr->histogram[i]++;
  mutex_unlock(&lock);
}


void * malloc(size_t size)
{
  static void * (*func)();
  void * ret;

  if(!func) {
    func = (void *(*)()) dlsym(RTLD_NEXT, "malloc");
  }

  ret = func(size);

  bump_counter(&mdata, size);
  
  return(ret);
}

void * calloc(size_t nelem, size_t elsize)
{
  static void * (*func)();
  void * ret;
  int i;

  if(!func)
    func = (void *(*)()) dlsym(RTLD_NEXT, "calloc");

  ret = func(nelem, elsize);

  for(i=0;i<nelem;i++)
      bump_counter(&cdata, elsize);
  
  return(ret);
}

void * realloc(void *ptr, size_t size)
{
  static void * (*func)();
  void * ret;

  if(!func)
    func = (void *(*)()) dlsym(RTLD_NEXT, "realloc");

  ret = func(ptr, size);
  bump_counter(&rdata, size);
  
  return(ret);

}


/*
 * Source Code: XOpenDisplay_interpose.c
 * Library interposer for XOpenDisplay() and XCloseDisplay.
 * cc -g -o XOpenDisplay_interpose.so -G -Kpic 
 * XOpenDisplay_interpose.c
 * setenv LD_PRELOAD $cwd/XOpenDisplay_interpose.so
 * run the app
 * unsetenv LD_PRELOAD
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <dlfcn.h>

Display *XOpenDisplay(char *display_name)
{
  static Display * (*func)(char *);
  Display *ret;
 
  if(!func)
    func = (Display *(*)(char *))dlsym(RTLD_NEXT, 
    "XOpenDisplay");

  if(display_name)
    printf("XOpenDisplay() is called with 
    display_name=%s\n", display_name);
  else
    printf("XOpenDisplay() is called with 
    display_name=NULL\n");
/*
  ret = func(display_name);
*/
  printf("  calling XOpenDisplay(NULL)\n");
  ret = func(0);
  printf("XOpenDisplay() returned %p\n", ret);

  return(ret);
}

int XCloseDisplay(Display *display_name)
{ 
  static int (*func)(Display *); 
  int ret; 
  
  if(!func)
    func = (int (*)(Display *))dlsym(RTLD_NEXT, 
    "XCloseDisplay"); 
 
  ret = func(display_name); 
 
  printf("called XCloseDisplay(%p)\n", display_name); 
 
  return(ret); 
}

