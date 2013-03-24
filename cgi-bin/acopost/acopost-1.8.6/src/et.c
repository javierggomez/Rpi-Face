/*
  Example-based Tagger

  Copyright (C) 2001 Ingo Schr�der

  Contact info:
  
    ingo@nats.informatik.uni-hamburg.de
    http://nats-www.informatik.uni-hamburg.de/~ingo/icopost/
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* ------------------------------------------------------------ */
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <strings.h> /* strtok */
#include "mem.h"
#include "array.h"
#include "hash.h"
#include "util.h"

char *strdup(const char *); /* not part of ANSI C */

/* ------------------------------------------------------------ */

#if 0
#define DEBUG(mode, msg...) report(mode, msg)
/*
  see http://gcc.gnu.org/onlinedocs/gcc-3.0.1/gcc_5.html#SEC82
  #define DEBUG(mode, msg...) fprintf (mode, msg, ## __VA_ARGS__)
*/
#else
#define DEBUG(mode, msg...) 
#endif

/* ------------------------------------------------------------ */

#define MODE_TAG 1
#define MODE_TEST 2

typedef struct globals_s
{
  int mode;
  char *cmd;    /* command name */   
  char *kf;     /* wtree file for known words */   
  char *uf;     /* wtree file for unknown words */   
  char *df;     /* dictionary file name */
  char *rf;     /* name of file to tag/test */
} globals_t;
typedef globals_t *globals_pt;

typedef struct option_s
{
  char ch;
  char *usage;
} option_t;
typedef option_t *option_pt;

#define FT_TAG 1
#define FT_CLASS 2
#define FT_WORD 3
#define FT_LETTER 4
#define FT_CAP 5
#define FT_HYPHEN 6
#define FT_NUMBER 7
#define FT_INTER 8

typedef struct feature_s
{
  int type;          /* type of feature, 0 WORD, 1 TAG ... */
  int arg1;          /* 1st para, e. g., WORD[-1] */
  int arg2;          /* 2nd para, e. g., LETTER[0,-1] */
  char *string;
  double weight;     /* weight */
  hash_pt v2i;       /* string -> index+1 */
  array_pt values;   /* list of values */
} feature_t;
typedef feature_t *feature_pt;

typedef struct value_s
{
  char *string;      /* literal value */
  int count;         /* how often have we seen this */
  int *tagcount;     /* array of ints */
} value_t;
typedef value_t *value_pt;

typedef struct wtree_s
{
  int defaulttag;     /* tag with largest tagcount[] */
  int *tagcount;      /* index -> count */
  char *id;
  feature_pt feature; /* feature corresponding to node */
  array_pt children;  /* index -> wtree */
} wtree_t;
typedef wtree_t *wtree_pt;

typedef struct word_s
{
  char *string;      /* grapheme */
  int count;         /* total number of occurances */
  int defaulttag;    /* most frequent tag index */
  int *tagcount;     /* maps tag index -> no. of occurances */
  char *aclass;      /* ambiguity class */
} word_t;
typedef word_t *word_pt;

typedef struct model_s
{
  array_pt tags;      /* tags */
  hash_pt taghash;    /* lookup table tags[taghash{"tag"}-1]="tag" */

  array_pt classes;   /* ambiguity classes */
  hash_pt classhash;  /* lookup table */

  array_pt features;  /* list of sorted features */

  hash_pt dictionary; /* dictionary: string->int (best tag) */
  wtree_pt known;     /* weighted tree for known words */
  wtree_pt unknown;   /* weighted tree for known words */
} model_t;
typedef model_t *model_pt;

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
char *banner=
"Example-based Tagger (c) Ingo Schr�der, schroeder@informatik.uni-hamburg.de";

globals_pt g;

option_t ops[]={
  { 'v', "-v v  verbosity [1]" },
  { '\0', NULL },
};

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
static globals_pt new_globals(globals_pt old)
{
  globals_pt g=(globals_pt)mem_malloc(sizeof(globals_t));

  if (old) { memcpy(g, old, sizeof(globals_t)); return g; }

  g->mode=MODE_TAG;
  g->cmd=g->kf=g->uf=g->df=g->rf=NULL;
  
  return g;
}

/* ------------------------------------------------------------ */
static model_pt new_model()
{
  model_pt m=(model_pt)mem_malloc(sizeof(model_t));
  memset(m, 0, sizeof(model_t));

  m->tags=array_new(64);
  m->taghash=hash_new(128, .5, hash_string_hash, hash_string_equal);

  m->classes=array_new(128);
  m->classhash=hash_new(256, .5, hash_string_hash, hash_string_equal);

  m->features=array_new(32);

  return m;
}

/* ------------------------------------------------------------ */
static word_pt new_word(char *s, int not)
{
  word_pt w=(word_pt)mem_malloc(sizeof(word_t));
  
  w->string=s;
  w->count=0;
  w->tagcount=(int *)mem_malloc(not*sizeof(int));
  memset(w->tagcount, 0, not*sizeof(int));

  return w;
}

/* ------------------------------------------------------------ */
static void delete_word(word_pt w)
{
  mem_free(w->tagcount);
  mem_free(w);
}

/* ------------------------------------------------------------ */
static feature_pt new_feature(void)
{
  feature_pt f=(feature_pt)mem_malloc(sizeof(feature_t));
  memset(f, 0, sizeof(feature_t));

  f->weight=0.0;
  f->v2i=hash_new(50, .5, hash_string_hash, hash_string_equal);
  f->values=array_new(32);
  
  return f;
}

/* ------------------------------------------------------------ */
static wtree_pt new_wtree(model_pt m)
{
  static char *foo42="foo42";
  int not=array_count(m->tags);
  wtree_pt t=(wtree_pt)mem_malloc(sizeof(wtree_t));
  memset(t, 0, sizeof(wtree_t));

  t->id=foo42;
  t->tagcount=(int *)mem_malloc(not*sizeof(int));
  memset(t->tagcount, 0, not*sizeof(int));
  t->children=array_new(32);

  return t;
}

/* ------------------------------------------------------------ */
static void usage(void)
{
  int i;
  report(-1, "\n%s\n\n", banner);
  report(-1, "Usage: %s OPTIONS knownwtree unknownwtree dictionary [inputfile]\n", g->cmd);
  report(-1, "where OPTIONS can be\n\n");
  for (i=0; ops[i].usage; i++)
    { report(-1, "  %s\n", ops[i].usage); }
  report(-1, "\n");
}

/* ------------------------------------------------------------ */
static void get_options(globals_pt g, int argc, char **argv)
{
  char c;

  while ((c=getopt(argc, argv, "tv:"))!=EOF)
    {
      switch (c)
	{
	case 't':
	  g->mode=MODE_TEST;
	  break;
	case 'v':
	  if (1!=sscanf(optarg, "%d", &verbosity))
	    { error("invalid verbosity \"%s\"\n", optarg); }
	  break;
	default:
	  error("unknown option \"-%c\"\n", c);
	  break;
	}
    }

  if (optind+2>=argc) { usage(); error("too few arguments\n"); }
  g->kf=strdup(argv[optind]);
  g->uf=strdup(argv[optind+1]);
  g->df=strdup(argv[optind+2]);
  if (optind+3<argc && strcmp("-", argv[optind+3]))
    { g->rf=strdup(argv[optind+3]); }
}

/* ------------------------------------------------------------ */
/* previously inlined */
static int find_tag(model_pt m, char *t)
{ return (int)hash_get(m->taghash, t)-1; }

/* ------------------------------------------------------------ */
static int register_tag(model_pt m, char *t)
{
  int i=find_tag(m, t);
 
  if (i<0) 
    { 
      t=strdup(t); 
      i=array_add(m->tags, t);
      hash_put(m->taghash, t, (void *)(i+1));
    }
  return i;
}

/* ------------------------------------------------------------ */
static void read_dictionary_file(model_pt m)
{
  FILE *f=try_to_open(g->df, "r");
  char *s;
#define BLEN 8000
  char b[BLEN];
  int lno, not, no_token=0;
  
  m->dictionary=hash_new(5000, .5, hash_string_hash, hash_string_equal);
  /* first pass through file: just get the tag */
  for (s=freadline(f); s; s=freadline(f)) 
    {
      for (s=strtok(s, " \t"), s=strtok(NULL, " \t");
	   s;
	   s=strtok(NULL, " \t"), s=strtok(NULL, " \t"))
	{ (void)register_tag(m, s); }
    }
  not=array_count(m->tags);

  /* rewind file */
  if (fseek(f, 0, SEEK_SET)) { error("can't rewind file \"%s\"\n", g->df); }

  /* second pass through file: collect details */
  for (lno=1, s=freadline(f); s; lno++, s=freadline(f)) 
    {
      int cnt;
      word_pt wd, old;
      
      s=strtok(s, " \t");
      if (!s) { report(1, "can't find word (%s:%d)\n", g->df, lno); continue; }
      s=register_string(s);
      wd=new_word(s, not);
      old=hash_put(m->dictionary, s, wd);
      if (old)
	{
	  report(1, "duplicate dictionary entry \"%s\" (%s:%d)\n", s, g->df, lno);
	  delete_word(old);
	}
      wd->defaulttag=-1;
      for (b[0]='*', b[1]='\0', s=strtok(NULL, " \t"); s;  s=strtok(NULL, " \t"))
	{
	  int ti=find_tag(m, s);
	  report(1, "Tag: %s\n", s);
	  if (ti<0)
	    { error("invalid tag \"%s\" (%s:%d)\n", s, g->df, lno); }
	  if (strlen(b)+strlen(s)+2>BLEN)
	    { error("oops, ambiguity class too long (%s:%d)\n", g->df, lno); }
	  strcat(b, s); strcat(b, "*");
	  s=strtok(NULL, " \t");
	  report(1, "count: %s\n", s);
	  if (!s || 1!=sscanf(s, "%d", &cnt))
	    { error("can't find tag count (%s:%d)\n", g->df, lno); }
	  wd->tagcount[ti]=cnt;
	  wd->count+=cnt;
	  if (wd->defaulttag<0) { wd->defaulttag=ti; }
	}
      wd->aclass=register_string(b);
      no_token+=wd->count;
    }
  report(2, "read %d/%d entries (type/token) from dictionary\n",
	 hash_size(m->dictionary), no_token);
}

/* ------------------------------------------------------------ */
static feature_pt parse_feature(model_pt m, char *t)
{
  feature_pt f=new_feature();
  
  t=strtok(t, " \t");
  if (!t) { return NULL; }
  f->string=strdup(t);
  t=strtok(NULL, " \t");
  if (!t) { return NULL; }
  if (1!=sscanf(t, "%lf", &f->weight)) { return NULL; }

  f->type=f->arg1=f->arg2=-1;
  if (1==sscanf(f->string, "TAG[%d]=", &f->arg1))
    { f->type=FT_TAG; }
  else if (1==sscanf(f->string, "CLASS[%d]=", &f->arg1))
    { f->type=FT_CLASS; }
  else if (1==sscanf(f->string, "WORD[%d]=", &f->arg1))
    { f->type=FT_WORD; }
  else if (2==sscanf(f->string, "LETTER[%d,%d]=", &f->arg1, &f->arg2))
    { f->type=FT_LETTER; }
  else if (1==sscanf(f->string, "CAP[%d]=", &f->arg1))
    { f->type=FT_CAP; }
  else if (1==sscanf(f->string, "HYPHEN[%d]=", &f->arg1))
    { f->type=FT_HYPHEN; }
  else if (1==sscanf(f->string, "NUMBER[%d]=", &f->arg1))
    { f->type=FT_NUMBER; }
  else if (1==sscanf(f->string, "INTER[%d]=", &f->arg1))
    { f->type=FT_INTER; }
  else { error("can't parse feature \"%s\"!\n", f->string); }

/*   report(0, "found feature %s type %d arg1 %d arg2 %d\n", f->string, f->type, f->arg1, f->arg2); */
  return f;
}

/* ------------------------------------------------------------ */
static int find_feature_value(feature_pt f, char *s)
{
  int fi=(int)hash_get(f->v2i, s)-1;
  DEBUG(3, "find_feature_value %s %s ==> %d\n", f->string, s, fi); 
  return fi;
}

/* ------------------------------------------------------------ */
static int register_feature_value(feature_pt f, char *s)
{
  int i=find_feature_value(f, s);
  if (i<0)
    {
      s=register_string(s);
      i=array_add(f->values, s);
      hash_put(f->v2i, s, (void *)i+1);
    }

  return i;
}

/* ------------------------------------------------------------ */
static int find_feature_value_from_sentence(model_pt m, feature_pt f, char **ws, int *ts, int i, int wno)
{
  int rp=i+f->arg1;  
  word_pt w;

  DEBUG(2, "find_feature_value_from_sentence: f=%s rp=%d i=%d arg1=%d arg2=%d\n", f->string, rp, i, f->arg1, f->arg2);
  
  if (rp<0 || rp>=wno) { return find_feature_value(f, "*BOUNDARY*"); }

  w=hash_get(m->dictionary, ws[rp]);
  switch (f->type)
    {
    case FT_TAG:
      return find_feature_value(f, array_get(m->tags, ts[rp]));
    case FT_CLASS:
      return !w ? -1 : find_feature_value(f, w->aclass);
    case FT_WORD:
      {
	int fi=find_feature_value(f, ws[rp]);
	return fi<0 ? find_feature_value(f, "*RARE*") : fi;
      }
    case FT_LETTER:
      {
	int slen=strlen(ws[rp]);
	if (slen<abs(f->arg2)) { return find_feature_value(f, "*NONE*"); }
	else if (f->arg2<0) { return find_feature_value(f, substr(ws[rp], slen+f->arg2, -1)); }
	else { return find_feature_value(f, substr(ws[rp], f->arg2-1, 1)); }
      }
    case FT_CAP:
      {
	char tmp[2]="X";
	int slen=strlen(ws[rp]);
	int clen=strspn(ws[rp], "ABCDEFGHIJKLMNOPQRSTUVWXYZ���");
	tmp[0]= slen==clen ? '2' : (clen>0 ? '1' : '0');
	return find_feature_value(f, tmp);
      }
    case FT_HYPHEN:
      {
	char tmp[2]="0";
	if (index(ws[rp], '-')) { tmp[0]='1'; }
	return find_feature_value(f, tmp);
      }
    case FT_NUMBER:
      {
	char tmp[2]="X";
	int slen=strlen(ws[rp]);
	char *dindex=strpbrk(ws[rp], "0123456789");
	int clen=strspn(ws[rp], "0123456789,.");
	int dlen=strspn(ws[rp], "0123456789");
	tmp[0]= slen==dlen ? '3' : ( slen==clen ? '2' : (dindex ? '1' : '0'));
	return find_feature_value(f, tmp);
      }
    case FT_INTER:
      {
	char tmp[2]="0";
	if (strspn(ws[rp], ",.;?!:")==strlen(ws[rp])) { tmp[0]='1'; }
	return find_feature_value(f, tmp);
      }
    default:
      error("unknown feature type %d\n", f->type);
    }
  return -1;
}

/* ------------------------------------------------------------ */
static void prune_wtree(model_pt m, wtree_pt t)
{
  int i;
  int not=array_count(m->tags);

  if (!t) { return; }
  for (i=0; i<array_count(t->children); i++)
    {
      wtree_pt child=array_get(t->children, i);
      int j;
      if (!child) { continue; }
      for (j=0; j<not; j++)
	{
	  t->tagcount[j]+=child->tagcount[j];
	  if (t->tagcount[t->defaulttag]<t->tagcount[j]) { t->defaulttag=j; }
	}
    }
  /* TODO: really prune the tree */
}

/* ------------------------------------------------------------ */
static wtree_pt read_wtree(model_pt m, char *fname)
{
  char *s;
  int lno, fno, cl=0, non=1, fos=array_count(m->features);
  FILE *f=try_to_open(fname, "r");
  wtree_pt root=new_wtree(m);
  wtree_pt *ns;
    
  /* first read list of features */
  for (lno=1, s=freadline(f);
       s && (!*s || (s[0]=='#' && s[1]=='#'));
       lno++, s=freadline(f))
    { /* skip over empty line and comments */ }
  for (/* nada */; s && *s; lno++, s=freadline(f))
    {
      feature_pt ft=parse_feature(m, s);
      if (!ft) { error("%s:%d: can't parse feature \"%s\"\n", fname, lno, s); }
      array_add(m->features, ft);
    }

  fno=array_count(m->features)-fos;
  if (fno<=0) { error("%s:%d: no features found\n", fname, lno); }
  root->feature=array_get(m->features, fos);
  
  /* this is the separating blank line */
  if (!s) { error("%s:%d: format error\n", fname, lno); }

  ns=(wtree_pt *)mem_malloc((fno+1)*sizeof(wtree_pt));
  memset(ns, 0, (fno+1)*sizeof(wtree_pt));
  ns[0]=root;
  
  /* now parse the tree */
  for (cl=1, lno++, s=freadline(f); s; lno++, s=freadline(f))
    {
      char *t;
      int i, l;
      wtree_pt mom, wt;
      
      if (!*s) { continue; }
      for (l=1; *s=='\t'; s++, l++) { /* nada */ }
      if (l>cl+1)
	{ error("%s:%d: skip of level %d>%d+1\n", fname, lno, l, cl); }
      for (i=cl; i>=l; i--) { prune_wtree(m, ns[i]); }
      cl=l;
      mom=ns[l-1];

      t=strtok(s, " \t");
      if (!t) { error("%s:%d: can't read value\n", fname, lno); }

      i=register_feature_value(mom->feature, t);
      /* i is index of value, wt is current node */
/*       report(-1, "t=%s l=%d cl=%d mom->feature=%s i=%d\n", t, l, cl, mom->feature->string, i); */
      
      if (i<array_count(mom->children) && array_get(mom->children, i))
	{ error("%s:%d: duplicate feature value %d %d\n", fname, lno, i, array_count(mom->children)); }

      wt=new_wtree(m);
      non++;
      if (l<fno) { wt->feature=array_get(m->features, l+fos); }
      array_set(mom->children, i, wt);
      for (t=strtok(NULL, " \t"); t; t=strtok(NULL, " \t"))
	{
	  int c, ti=register_tag(m, t);

	  /* leaf node */
	  t=strtok(NULL, " \t");
	  if (!t) { error("%s:%d: can't find tag count\n", fname, lno); }
	  if (1!=sscanf(t, "%d", &c))
	    { error("%s:%d: tag count not a number\n", fname, lno); }
	  wt->tagcount[ti]=c;
	  if (c>wt->tagcount[wt->defaulttag]) { wt->defaulttag=ti; }
	}
      ns[l]=wt;
    }
  for (fno=cl; fno>=0; fno--) { prune_wtree(m, ns[fno]); }
  mem_free(ns);

  report(1, "read wtree with %d nodes from \"%s\"\n", non, fname);
  
  return root;
}

/* ------------------------------------------------------------ */
static void read_known_wtree(model_pt m)
{ m->known=read_wtree(m, g->kf); }

/* ------------------------------------------------------------ */
static void read_unknown_wtree(model_pt m)
{ m->unknown=read_wtree(m, g->uf); }

/* ------------------------------------------------------------ */
void print_wtree(wtree_pt t, int indent)
{
  feature_pt f=t->feature;
  int i;
  
  for (i=0; i<array_count(t->children); i++)
    {
      wtree_pt son=(wtree_pt)array_get(t->children, i);
      char *v=(char *)array_get(f->values, i);
      int j;
      if (!son) { continue; }
      for (j=0; j<indent; j++) { report(-1, " "); }
      report(-1, "%s\n", v);
      print_wtree(son, indent+2);
    }
}

/* ------------------------------------------------------------ */
static void tagging(model_pt m)
{
  int not=array_count(m->tags), nop=0;
  FILE *f= g->rf ? try_to_open(g->rf, "r") : stdin;
  char **words=NULL;
  int *tags=NULL;
  char *s;

  for (s=freadline(f); s; s=freadline(f))
    {
      char *t;
      int i, wno;
      DEBUG(2, "GOT %s\n", s);
      for (wno=0, t=strtok(s, " \t"); t; wno++, t=strtok(NULL, " \t"))
	{
	  if (wno>=nop)
	    {
	      if (nop<=0) { nop=128; }
	      while (wno>=nop) { nop*=2; }
	      words=(char **)mem_realloc(words, nop*sizeof(char *));
	      tags=(int *)mem_realloc(tags, nop*sizeof(int));
	    }
 	  report(4, "word %d %s\n", wno, t);
	  words[wno]=t;
	}
      /* Now we have the sentence available. */
      for (i=0; i<wno; i++)
	{
	  char *word=words[i];
	  word_pt w=hash_get(m->dictionary, word);
	  wtree_pt tree= w ? m->known : m->unknown;

	  report(4, "word %s is %sknown.\n", word, w ? "" : "un");
	  while (1)
	    {
	      feature_pt f=tree->feature;
	      wtree_pt next;
	      int fi;

	      if (!f) { report(4, "leaf node reached, breaking out\n"); break; }

	      fi=find_feature_value_from_sentence(m, f, words, tags, i, wno);
	      if (fi<0) { report(4, "can't find value, breaking out\n"); break; }
	      if (fi>=array_count(tree->children))
		{ report(4, "can't find child %d>=%d, breaking out\n", fi, array_count(tree->children)); break; }
	      next=(wtree_pt)array_get(tree->children, fi);
	      if (!next) { report(4, "can't find child for %d, breaking out\n", fi); break; }
	      tree=next;
	      {
		int j;
		report(4, "  current node %p %d %s :::",
		       tree, tree->defaulttag, (char *)array_get(m->tags, tree->defaulttag));
		for (j=0; j<not; j++)
		  {
		    if (tree->tagcount[j]<=0) { continue; }
		    report(-4, " %s:%d", (char *)array_get(m->tags, j), tree->tagcount[j]);
		  }
		report(-4, "\n");
	      }
	    }
	  tags[i]=tree->defaulttag;
	  if (i>0) { printf(" "); }
	  printf("%s %s", word, (char *)array_get(m->tags, tags[i]));
	  report(4, "OUT: %s %s\n", word, (char *)array_get(m->tags, tags[i]));
	}
      printf("\n");
    }
  if (words) { mem_free(words); mem_free(tags); }  
  if (f!=stdin) { fclose(f); }
}

/* ------------------------------------------------------------ */
void testing(model_pt m)
{
  char s[4000];
  int not=array_count(m->tags), i;
  
  report(-1, "Enter word followed by features values.\n");
  while (fgets(s, 1000, stdin))
    {
      char *w=strtok(s, " \t\n");
      word_pt wd=hash_get(m->dictionary, w);
      wtree_pt t= wd ? m->known : m->unknown;
      if (wd) { report(-1, "word %s defaulttag %d %s\n", w, wd->defaulttag, (char *)array_get(m->tags, wd->defaulttag)); }
      while ((w=strtok(NULL, " \t\n")))
	{
	  feature_pt f=t->feature;
	  int fi=find_feature_value(f, w);
	  if (fi<0) { report(1, "can't find \"%s\", breaking out\n", w); break; }
	  t=(wtree_pt)array_get(t->children, fi);
	  if (!t) { report(1, "can't find child for %d, breaking out\n", fi); break; }
	  report(1, "current node %p %d %s :::",
		 t, t->defaulttag, (char *)array_get(m->tags, t->defaulttag));
	  for (i=0; i<not; i++)
	    {
	      if (t->tagcount[i]>0)
		{ report(-1, " %s:%d", (char *)array_get(m->tags, i), t->tagcount[i]); }
	    }
	  report(-1, "\n");
	}
    }  
  
}

/* ------------------------------------------------------------ */
int main(int argc, char **argv)
{ 
  model_pt m=new_model();

  report(1, "\n");
  report(1, "%s\n", banner);
  report(1, "\n");

  g=new_globals(NULL);
  g->cmd=strdup(basename(argv[0], NULL));
  get_options(g, argc, argv);
  
  read_dictionary_file(m);

  read_known_wtree(m);
  read_unknown_wtree(m);

  switch (g->mode)
    {
    case MODE_TAG: tagging(m); break;
    case MODE_TEST: testing(m); break;
    default: error("unknown mode of operation %d\n", g->mode);
    }
  
  exit(0);
}

/* ------------------------------------------------------------ */