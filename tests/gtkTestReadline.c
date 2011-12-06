#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#define INTERVAL 1000

enum {TIMER = 0, IDLE};
#define NUM_INIT_ENTRIES 3

typedef struct {
  gboolean isActive;
  gint     id;
  gint     index;
  gint     count;
} State;

GtkWidget *textLog;
GtkWidget *lists[2];

GtkWidget *createGUI(int, char **);
void addEntries(int numEntries);


void addReadlineHandler();

int
main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  createGUI(argc, argv);
  addEntries(NUM_INIT_ENTRIES);
  addReadlineHandler();

  gtk_main();

  return(0);
}


void
showEvent(State *data, int type)
{
  if(data->count++ % 10 == 0) {
    char buf[300];
    sprintf(buf, "%s: %d %d\n", type == TIMER ? "timer" : "idle", data->index, data->count);
    gtk_text_insert(GTK_TEXT(textLog), NULL, NULL, NULL, buf, strlen(buf)); 
  }
}

gint
idleFun(State *data)
{
  showEvent(data, IDLE);
  return(1);
}

gint
timerFun(State *data)
{
  showEvent(data, TIMER);
  return(1);
}


void
addIdleCB(GtkWidget *w, State *data)
{
  if(data->id) {
    gtk_idle_remove(data->id);     
    data->id = 0;
#if TO_STDERR
    fprintf(stderr, "Removing idle\n");fflush(stderr);
#endif
  } else {
#if TO_STDERR
    data->id = gtk_idle_add(idleFun, (gpointer) data);
#endif
  }
}


void
addTimeoutCB(GtkWidget *w, State *data)
{
  if(data->id) {
    gtk_timeout_remove(data->id);     
    data->id = 0;
#if TO_STDERR
    fprintf(stderr, "Removing timer\n");fflush(stderr);
#endif
  } else {
#if TO_STDERR
    fprintf(stderr, "Adding timer\n");fflush(stderr);
#endif
    data->id = gtk_timeout_add(INTERVAL, timerFun, (gpointer) data);
  }
}


GtkWidget *
createGUI(int argc, char **argv)
{
  GtkWidget *win;
  GtkWidget *sw, *obox, *box1;
  int i;
  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);


  obox = gtk_hbox_new(TRUE, 0);
  box1 = gtk_hbox_new(TRUE, 0);
  for(i = 0; i < 2; i++) {
    sw = gtk_scrolled_window_new(NULL, NULL);
    lists[i] = gtk_list_new();
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), lists[i]);
    gtk_box_pack_start(GTK_BOX(box1), sw, TRUE, TRUE, 0);
  }

  gtk_box_pack_start(GTK_BOX(obox), box1, TRUE, TRUE, 0);
  sw = gtk_scrolled_window_new(NULL, NULL);
  textLog = gtk_text_new(NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(textLog), TRUE);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), textLog);

  gtk_box_pack_end(GTK_BOX(obox), sw, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(win), obox);
  gtk_widget_show_all(win);    

  return(win);
}



void
addEntries(int numEntries)
{
  void (*f[])(GtkWidget *, State *) = {&addTimeoutCB, &addIdleCB};
  GList *tmpList = NULL;
  int i, j;
  GtkWidget *label;
  char buf[100];
  State *state;

  for(i = 0; i < 2 ; i++) {
    tmpList = NULL;
    for(j = 0; j < numEntries; j++) {
	state = (State *) g_malloc(sizeof(State));
        state->index = j;
	state->id = 0;
	state->isActive = FALSE;
	state->count = 0;

/*	f[i](NULL, state); */
	sprintf(buf, "%s %d", i == TIMER ? "timer" : "idle", j + 1);
	label = gtk_list_item_new_with_label((char *) buf);
	gtk_widget_show(label);
	gtk_signal_connect(GTK_OBJECT(label), "select", f[i], state);
        tmpList = g_list_append(tmpList, label);
    }
    gtk_list_append_items(GTK_LIST(lists[i]), tmpList);
  }
}


