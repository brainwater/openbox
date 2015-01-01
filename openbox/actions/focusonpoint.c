#include "openbox/actions.h"
#include "openbox/event.h"
#include "openbox/client.h"
#include "openbox/focus.h"
#include "openbox/screen.h"
#include "openbox/window.h"

typedef struct {
    gint x;
    gint x_denom;
    gint y;
    gint y_denom;
    gboolean raise;
} Options;

static gpointer setup_func(xmlNodePtr node);
static void free_func(gpointer o);
static gboolean run_func(ObActionsData *data, gpointer options);

void action_focusonpoint_startup(void)
{
    actions_register("FocusOnPoint", setup_func, free_func, run_func);
}

static gpointer setup_func(xmlNodePtr node)
{
    xmlNodePtr n;
    Options *o;
    gchar *s;

    o = g_slice_new0(Options);
    o->x = 0;
    o->x_denom = 0;
    o->y = 0;
    o->y_denom = 0;
    o->raise = TRUE;

    if ((n = obt_xml_find_node(node, "x"))) {
      s = obt_xml_node_string(n);
      config_parse_relative_number(s, &o->x, &o->x_denom);
      g_free(s);
    }
    if ((n = obt_xml_find_node(node, "y"))) {
      s = obt_xml_node_string(n);
      config_parse_relative_number(s, &o->y, &o->y_denom);
      g_free(s);
    }
    if ((n = obt_xml_find_node(node, "raise"))) {
      o->raise = obt_xml_node_bool(n);
    }
	
    return o;
}

static void free_func(gpointer o)
{
    g_slice_free(Options, o);
}

/* Always return FALSE because its not interactive */
static gboolean run_func(ObActionsData *data, gpointer options)
{
  Options *o = options;


  gint x, y;
  x = o->x;
  y = o->y;

  if (o->x_denom || o->y_denom) {
    const Rect *darea = screen_physical_area_active();
    if (o->x_denom) {
      x = (x * darea->width) / o->x_denom;
    }
    if (o->y_denom) {
      y = (y * darea->height) / o->y_denom;
    }
  }

  ObClient *client = client_at_point(x, y);
  
  if (NULL == client) {
    /* focus on root window */
    focus_nothing();
  } else {
    // TODO: Do I need the actions_client_move wrapping the focus?
    client_activate(client, TRUE, FALSE, FALSE, FALSE, TRUE);
    // If raise is true, raise the window to the front of the stack
    if (o->raise) {
      stacking_raise(CLIENT_AS_WINDOW(client));
    }
  }
  return FALSE;
}
