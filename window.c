#include <gtk/gtk.h>

// Definition of the TuasWindow subclass

#define TUAS_TYPE_WINDOW (tuas_window_get_type())
G_DECLARE_FINAL_TYPE(TuasWindow, tuas_window, TUAS, WINDOW, GtkApplicationWindow)
typedef struct _TuasWindow {
  GtkApplicationWindow parent;
  GtkWidget* columnview;
  GListStore* liststore;
} TuasWindow;
G_DEFINE_TYPE(TuasWindow, tuas_window, GTK_TYPE_APPLICATION_WINDOW)

// End of declaration

// Definition of JigData

enum {
  PROP_0,
  PROP_STRING,
  N_PROPERTIES
};
static GParamSpec *jig_data_properties[N_PROPERTIES] = {NULL};

#define JIG_TYPE_DATA (jig_data_get_type())
G_DECLARE_FINAL_TYPE(JigData, jig_data, JIG, DATA, GObject)
typedef struct _JigData {
  GObject parent;
  GtkListItem* item;
  char* desc;
} JigData;
G_DEFINE_TYPE(JigData, jig_data, G_TYPE_OBJECT)

static void jig_data_init(JigData* self) {
  self->item = NULL;
  self->desc = NULL;
}

static void jig_data_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_STRING) {
    self->desc = g_strdup(g_value_get_string(value));
  } else {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void jig_data_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_STRING) {
    g_value_set_string(value, g_strdup(self->desc));
  } else {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void jig_data_finalize(GObject* object) {
  JigData* self = JIG_DATA(object);
  if (self->desc) {
    g_free(self->desc);
  }
  G_OBJECT_CLASS(jig_data_parent_class)->finalize(object);
}

static void jig_data_class_init(JigDataClass* class) {
  GObjectClass* gobject_class = G_OBJECT_CLASS(class);
  gobject_class->finalize = jig_data_finalize;
  gobject_class->set_property = jig_data_set_property;
  gobject_class->get_property = jig_data_get_property;
  jig_data_properties[PROP_STRING] = g_param_spec_string("string", "string", "string", "", G_PARAM_READWRITE);
  g_object_class_install_properties(gobject_class, N_PROPERTIES, jig_data_properties);
}

JigData* jig_data_new_with_data(GtkListItem* item, const char* desc) {
  g_return_val_if_fail(GTK_IS_LIST_ITEM(item) || item == NULL, NULL);
  JigData *data = JIG_DATA(g_object_new(JIG_TYPE_DATA, NULL));
  data->item = item ? g_object_ref(item) : NULL;
  data->desc = g_strdup(desc);
  return data;
}

// End of declaration

// Row addition callback
static void new_row_cb(GtkButton* btn, TuasWindow* win) {
  JigData* data = jig_data_new_with_data(NULL, "");
  g_list_store_append(win->liststore, data);
  g_object_unref(data);
}

void setup_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* text = gtk_entry_new();
  gtk_list_item_set_child(item, GTK_WIDGET(text));
}

void bind_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* text = gtk_list_item_get_child(item);
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(text));
  gtk_entry_set_placeholder_text(GTK_ENTRY(text), "Jig description");
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  GBinding* bind = g_object_bind_property(buffer, "text", data, "string", G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

static void unbind_desc_cb(GtkListItemFactory* factory, GtkListItem* listitem) {
  GBinding* bind = G_BINDING(g_object_get_data(G_OBJECT(listitem), "bind"));
  g_binding_unbind(bind);
  g_object_set_data (G_OBJECT(listitem), "bind", NULL);
}

// Begin tedious boilerplate

static void tuas_window_init(TuasWindow* win) {
  gtk_widget_init_template(GTK_WIDGET(win));
}

static void tuas_window_class_init(TuasWindowClass* class) {
  // GObjectClass* gobject_class = G_OBJECT_CLASS(class);
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/smrt/Tuas/window.ui");
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), TuasWindow, columnview);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), TuasWindow, liststore);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), new_row_cb);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), setup_desc_cb);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), bind_desc_cb);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), unbind_desc_cb);
}

GtkWidget* tuas_window_new(GtkApplication* app) {
  g_return_val_if_fail(GTK_IS_APPLICATION(app), NULL);
  return GTK_WIDGET(g_object_new(TUAS_TYPE_WINDOW, "application", app, NULL));
}

static void app_activate(GApplication* application) {
  GtkApplication* app = GTK_APPLICATION (application);
  gtk_window_present(gtk_application_get_active_window(app));
}

static void app_startup(GApplication* application) {
  GtkApplication* app = GTK_APPLICATION (application);
  tuas_window_new(app);
}

// End tedious boilerplate

// Use `meson setup build; ninja -C build; build/window` to compile and run

int main(int argc, char* argv[]) {
  GtkApplication* app = gtk_application_new("org.smrt.Tuas", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
