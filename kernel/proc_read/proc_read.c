/*
 * https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
 * Simple demonstration of the seq_file interface.
 *
 * $Id: seq.c,v 1.1 2003/02/10 21:02:02 corbet Exp $
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

MODULE_AUTHOR("Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

#define ORIGINAL  0
#define ORIGINALM 0
#define LIST_IMPL 1

#if(ORIGINALM)
#define CHART_SEQ 4
char* titles[CHART_SEQ] = {"title 1", "title 2", "title 3", "title 4"};
#endif

#if(LIST_IMPL)
struct objekt_listy {
    struct list_head in_lh;
    int index;
    char nazwa[16];
} g_o1, g_o2, g_o3;

LIST_HEAD(lista);
#endif

/*
 * The sequence iterator functions.  We simply use the count of the
 * next line as our internal position.
 */
#if(ORIGINAL)
static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
    loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
    if (! spos)
        return NULL;
    *spos = *pos;
    return spos;
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    loff_t *spos = (loff_t *) v;
    *pos = ++(*spos);
    return spos;
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
    kfree (v);
}

/*
 * The show function.
 */
static int ct_seq_show(struct seq_file *s, void *v)
{
    loff_t *spos = (loff_t *) v;
    seq_printf(s, "%Ld\n", *spos);
    return 0;
}
#elif(LIST_IMPL)
static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
    return seq_list_start(&lista, *pos);
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    return seq_list_next(v, &lista, pos);

}

static void ct_seq_stop(struct seq_file *s, void *v)
{
}

/*
 * The show function.
 */
static int ct_seq_show(struct seq_file *s, void *v)
{
    struct objekt_listy *ol = (struct objekt_listy *)v;
    seq_printf(s, "(%d, %s)\n", ol->index, ol->nazwa);
    return 0;
}
#elif(ORIGINALM)
static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
    return (*pos == 0) ? ((void*)(titles[(*pos)])) : ((void*)0);
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    if(++(*pos) < CHART_SEQ)
        v = titles[(*pos)];
    else
        v = 0;
    return v;
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
}

/*
 * The show function.
 */
static int ct_seq_show(struct seq_file *s, void *v)
{
    int i;
    char *title = (char *)v;
    seq_printf(s, "[%s]\n", title);
    for(i=0;i<50;i++)
        seq_printf(s, "%d,", i);
    seq_printf(s, "\n");
    return 0;
}
#endif

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations ct_seq_ops = {
    .start = ct_seq_start,
    .next  = ct_seq_next,
    .stop  = ct_seq_stop,
    .show  = ct_seq_show
};


/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */

static int ct_open(struct inode *inode, struct file *file)
{
#if(LIST_IMPL)
    struct seq_file *m;
    int ret;

    ret = seq_open(file, &ct_seq_ops);
    if(ret<0)
        return ret;

    m = file->private_data;
    m->private = &lista;
    return 0;
#else /* ORIGINAL albo ORIGINALM */
    return seq_open(file, &ct_seq_ops);
#endif
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
static struct file_operations ct_file_ops = {
    .owner   = THIS_MODULE,
    .open    = ct_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = seq_release
};
  
  
/*
 * Module setup and teardown.
 */

static int ct_init(void)
{
    struct proc_dir_entry *entry;

#if(LIST_IMPL)
    g_o1.index=10;
    strcpy(g_o1.nazwa, "Jedna dyszka");
    g_o2.index=20;
    strcpy(g_o2.nazwa, "Dwie dyszki");
    g_o3.index=30;
    strcpy(g_o3.nazwa, "III dyszki");

    /* Adding to the list */
    list_add((struct list_head *)&g_o1, &lista);
    list_add((struct list_head *)&g_o2, &lista);
    list_add((struct list_head *)&g_o3, &lista);
#endif

    entry = proc_create("sequence", 0, NULL, &ct_file_ops);

    return 0;
}

static void ct_exit(void)
{
    remove_proc_entry("sequence", NULL);
}

module_init(ct_init);
module_exit(ct_exit);
