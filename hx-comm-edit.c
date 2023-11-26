/* hx-comm-edit.c: Comment/uncomment stream from Helix editor */
#include <stdio.h>
#include <stdlib.h>

#define LANG_MAX_LENGTH 20
#define COMM_START_MAX_LENGTH 10
#define COMM_END_MAX_LENGTH 10

FILE *file;
int lang_length, start_length, end_length;

void open_file(const char *argv);
int get_lang(char *lang, const char *argv);
int extract_comm(char *comm, int switcher);
void get_comm(char *start, char *end, int lang_l, char *lang,
              const char *argv);
void edit_comm(char *start, char *end);

int main(int argc, char **argv) {
  char lang[LANG_MAX_LENGTH];
  char start[COMM_START_MAX_LENGTH];
  char end[COMM_END_MAX_LENGTH];
  lang_length=get_lang(lang, argv[1]);
  get_comm(start, end, lang_length, lang, argv[2]);
  edit_comm(start, end);
  return 0;
}

void open_file(const char *argv) {
  if ((file = fopen(argv, "r")) == NULL) {
    fprintf(stderr, "Can't open file.\n");
    exit(1);
  }
}

int get_lang(char *lang, const char *argv) {
  int chr, i = 0;
  open_file(argv);
  while ((chr = getc(file)) != EOF && chr != '\n') {
    lang[i] = chr; ++i;
  }
  lang[i] = '\0';
  fclose(file);
  return i;
}

int extract_comm(char *comm, int switcher) {
  int chr, count = 0;
  if (switcher) {
    comm[count] = ' '; ++count;
  }
  while ((chr = getc(file)) == '\t' || chr == ' ');
  comm[count] = chr;
  while ((chr = getc(file)) != '\t' && chr != ' '
    && chr != '\n' && chr != EOF) {
    ++count; comm[count] = chr;
  }
  if (!switcher) {
    ++count; comm[count] = ' ';
  }
  ++count; comm[count] = '\0';
  if (switcher) end_length = count;
  else start_length = count;
  return chr;
}

void get_comm(char *start, char *end, int lang_l, char *lang,
              const char *argv) {
  int chr, count = 0, flag = 0;
  open_file(argv);
  while ((chr = getc(file)) != EOF) {
    if (chr == lang[count]) {
      ++flag; ++count;
      if (flag == lang_l) {
        chr = extract_comm(start, 0);
        if (chr == '\n' || chr == EOF) {
          end_length = 0; end[0] = '\0';
          break;
        } else {
          extract_comm(end, 1);
          break;
        }
      } else {
        continue;
      }
    } else if (chr == '*') {
      chr = extract_comm(start, 0);
      if (chr == '\n' || chr == EOF) {
        end_length = 0; end[0] = '\0';
      } else {
        extract_comm(end, 1);
      }
    } else {
      while ((chr = getc(file)) != '\n');
      count = 0; flag = 0;
    }
  }
  fclose(file);
}

void edit_comm(char *start, char *end) {
  int chr, start_count = 0, start_done = 0,
    end_get = 0, end_count = 0, end_done = 0,
      unget_chr = 0;
  while (unget_chr || (chr = getc(stdin)) != EOF) {
    unget_chr = 0;
    if (chr == '\n') {
      if (end_length && start_done && !end_get)
        for (int i = 0; i < end_length; ++i)
          putc(end[i], stdout);
      start_count = start_done = end_get = end_count = end_done = 0;
      putc(chr, stdout); continue;
    }
    if (!start_done) {
      if (chr == start[start_count]) {
        ++start_count;
        if (start_count == start_length) {
          start_done = 1; 
          end_get = 1;
        }
      } else if (chr == '\t' || chr == ' ') putc(chr, stdout);
      else {
        for (int i = 0; i < start_length; ++i)
          putc(start[i], stdout);
        if (start_count) for (int i = 0; i < start_count; ++i)
          putc(start[i], stdout);
          putc(chr, stdout); start_done = 1;
      }
    } else if ((chr == end[end_count]) && end_length && !end_done && end_get) {
      ++end_count;
      if (end_count == end_length) end_done = 1;
    } else if (end_count) {
      for (int i = 0; i < end_count; ++i) putc(end[i], stdout);
      end_count = 0;
      unget_chr = 1;
    } else putc(chr, stdout);
  }
  if (start_done && !end_done && !end_get)
    for (int i = 0; i < end_length; ++i) putc(end[i], stdout);
}
