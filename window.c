#include <gtk/gtk.h>

// Definition of the TuasWindow subclass

#define TUAS_TYPE_WINDOW (tuas_window_get_type())
G_DECLARE_FINAL_TYPE(TuasWindow, tuas_window, TUAS, WINDOW, GtkApplicationWindow)
typedef struct _TuasWindow {
  GtkApplicationWindow parent;
  GtkColumnView* columnview;
  GListStore* liststore;
} TuasWindow;
G_DEFINE_TYPE(TuasWindow, tuas_window, GTK_TYPE_APPLICATION_WINDOW)

// End of declaration

// Definition of JigData

enum {
  PROP_DESC = 1,
  PROP_STATUS0 = 2,
  PROP_STATUS1 = 3,
  PROP_STATUS2 = 4,
  PROP_STATUS3 = 5,
  PROP_STATUS4 = 6,
  N_PROPERTIES
};
static GParamSpec *jig_data_properties[N_PROPERTIES] = {NULL};

#define JIG_TYPE_DATA (jig_data_get_type())
G_DECLARE_FINAL_TYPE(JigData, jig_data, JIG, DATA, GObject)
typedef struct _JigData {
  GObject parent;
  GtkListItem* item;
  char* desc;
  char* status[5];
} JigData;
G_DEFINE_TYPE(JigData, jig_data, G_TYPE_OBJECT)

static void jig_data_init(JigData* self) {
  self->item = NULL;
  self->desc = NULL;
  for (int i = 0; i < 5; ++i) {
    self->status[i] = NULL;
  }
}

static void jig_data_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_DESC) {
    self->desc = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_STATUS0) {
    self->status[0] = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_STATUS1) {
    self->status[1] = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_STATUS2) {
    self->status[2] = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_STATUS3) {
    self->status[3] = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_STATUS4) {
    self->status[4] = g_strdup(g_value_get_string(value));
  } else {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void jig_data_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_DESC) {
    g_value_set_string(value, g_strdup(self->desc));
  } else if (property_id == PROP_STATUS0) {
    g_value_set_string(value, g_strdup(self->status[0]));
  } else if (property_id == PROP_STATUS1) {
    g_value_set_string(value, g_strdup(self->status[1]));
  } else if (property_id == PROP_STATUS2) {
    g_value_set_string(value, g_strdup(self->status[2]));
  } else if (property_id == PROP_STATUS3) {
    g_value_set_string(value, g_strdup(self->status[3]));
  } else if (property_id == PROP_STATUS4) {
    g_value_set_string(value, g_strdup(self->status[4]));
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
  jig_data_properties[PROP_DESC] = g_param_spec_string("desc", "string", "string", "", G_PARAM_READWRITE);
  jig_data_properties[PROP_STATUS0] = g_param_spec_string("status0", "string", "string", "", G_PARAM_READWRITE);
  jig_data_properties[PROP_STATUS1] = g_param_spec_string("status1", "string", "string", "", G_PARAM_READWRITE);
  jig_data_properties[PROP_STATUS2] = g_param_spec_string("status2", "string", "string", "", G_PARAM_READWRITE);
  jig_data_properties[PROP_STATUS3] = g_param_spec_string("status3", "string", "string", "", G_PARAM_READWRITE);
  jig_data_properties[PROP_STATUS4] = g_param_spec_string("status4", "string", "string", "", G_PARAM_READWRITE);
  g_object_class_install_properties(gobject_class, N_PROPERTIES, jig_data_properties);
}

JigData* jig_data_new_with_data(GtkListItem* item, char* desc,
    char* status0, char* status1, char* status2, char* status3, char* status4) {
  g_return_val_if_fail(GTK_IS_LIST_ITEM(item) || item == NULL, NULL);
  JigData *data = JIG_DATA(g_object_new(JIG_TYPE_DATA, NULL));
  data->item = item ? g_object_ref(item) : NULL;
  data->desc = g_strdup(desc);
  data->status[0] = g_strdup(status0);
  data->status[1] = g_strdup(status1);
  data->status[2] = g_strdup(status2);
  data->status[3] = g_strdup(status3);
  data->status[4] = g_strdup(status4);
  return data;
}

// End of declaration

// Row addition callback
static void new_row_cb(GtkButton* btn, TuasWindow* win) {
  JigData* data = jig_data_new_with_data(NULL, "", "Open", "Open", "Open", "Open", "Open");
  g_list_store_append(win->liststore, data);
  g_object_unref(data);
}

// Write callback
static void write_cb(GtkButton* btn, TuasWindow* win) {
  GListModel* lm = G_LIST_MODEL(win->liststore);
  JigData* item;
  g_print("%u items\n", g_list_model_get_n_items(lm));
  for (int i = 0; (item = JIG_DATA(g_list_model_get_item(lm, i))) != NULL; ++i) {
    g_print("\"%s\" =>", item->desc);
    for (int j = 0; j < 5; ++j) {
      g_print(" %s", item->status[j]);
    }
    g_print("\n");
  }
}

// Description callbacks
void setup_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* text = gtk_entry_new();
  gtk_list_item_set_child(item, text);
}

void bind_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkEntry* entry = GTK_ENTRY(gtk_list_item_get_child(item));
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
  gtk_entry_set_placeholder_text(entry, "Jig description");
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  GBinding* bind = g_object_bind_property(buffer, "text", data, "desc", G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

void unbind_desc_cb(GtkListItemFactory* factory, GtkListItem* listitem) {
  GBinding* bind = G_BINDING(g_object_get_data(G_OBJECT(listitem), "bind"));
  g_binding_unbind(bind);
  g_object_set_data(G_OBJECT(listitem), "bind", NULL);
}

// Parameter button callbacks
void setup_status_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* btn = gtk_button_new();
  gtk_list_item_set_child(item, btn);
}

void state_change_cb(GtkButton* btn) {
  const char* label = gtk_button_get_label(btn);
  if (g_str_equal(label, "Open")) {
    gtk_button_set_label(btn, "Closed");
  } else if (g_str_equal(label, "Closed")) {
    gtk_button_set_label(btn, "N/A");
  } else if (g_str_equal(label, "N/A")) {
    gtk_button_set_label(btn, "Open");
  }
}

void bind_status_cb(GtkListItemFactory* self, GtkListItem* item, gpointer i) {
  GtkButton* btn = GTK_BUTTON(gtk_list_item_get_child(item));
  gtk_button_set_label(btn, "Open");
  g_signal_connect(btn, "clicked", G_CALLBACK(state_change_cb), NULL);
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  GBinding* bind = g_object_bind_property(btn, "label", data, "status1", G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

// Begin tedious boilerplate

char* colnames[] = {"User reqs", "Design", "Testing", "AFM", "Report"};

void tuas_window_init(TuasWindow* win) {
  gtk_widget_init_template(GTK_WIDGET(win));
  for (int i = 0; i < 5; ++i) {
    GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_status_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_status_cb), GINT_TO_POINTER(i));
    GtkColumnViewColumn* column = gtk_column_view_column_new(g_strdup(colnames[i]), factory);
    gtk_column_view_append_column(win->columnview, column);
  }
}

void tuas_window_class_init(TuasWindowClass* class) {
  GtkWidgetClass* wc = GTK_WIDGET_CLASS(class);
  gtk_widget_class_set_template_from_resource(wc, "/org/smrt/Tuas/window.ui");
  gtk_widget_class_bind_template_child(wc, TuasWindow, columnview);
  gtk_widget_class_bind_template_child(wc, TuasWindow, liststore);
  gtk_widget_class_bind_template_callback(wc, new_row_cb);
  gtk_widget_class_bind_template_callback(wc, setup_desc_cb);
  gtk_widget_class_bind_template_callback(wc, bind_desc_cb);
  gtk_widget_class_bind_template_callback(wc, unbind_desc_cb);
  gtk_widget_class_bind_template_callback(wc, write_cb);
}

GtkWidget* tuas_window_new(GtkApplication* app) {
  g_return_val_if_fail(GTK_IS_APPLICATION(app), NULL);
  return GTK_WIDGET(g_object_new(TUAS_TYPE_WINDOW, "application", app, NULL));
}

void app_activate(GApplication* application) {
  GtkApplication* app = GTK_APPLICATION (application);
  gtk_window_present(gtk_application_get_active_window(app));
}

void app_startup(GApplication* application) {
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
