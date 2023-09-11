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
  PROP_LUPDATE = 7,
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
  char* lupdate;
} JigData;
G_DEFINE_TYPE(JigData, jig_data, G_TYPE_OBJECT)

static void jig_data_init(JigData* self) {
  self->item = NULL;
  self->desc = NULL;
  for (int i = 0; i < 5; ++i) {
    self->status[i] = NULL;
  }
  self->lupdate = NULL;
}

static void jig_data_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_DESC) {
    self->desc = g_strdup(g_value_get_string(value));
  } else if (PROP_STATUS0 <= property_id && property_id <= PROP_STATUS4) {
    self->status[property_id - PROP_STATUS0] = g_strdup(g_value_get_string(value));
  } else if (property_id == PROP_LUPDATE) {
    self->lupdate = g_strdup(g_value_get_string(value));
  } else {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void jig_data_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec) {
  JigData* self = JIG_DATA(object);
  if (property_id == PROP_DESC) {
    g_value_set_string(value, g_strdup(self->desc));
  } else if (PROP_STATUS0 <= property_id && property_id <= PROP_STATUS4) {
    g_value_set_string(value, g_strdup(self->status[property_id - PROP_STATUS0]));
  } else if (property_id == PROP_LUPDATE) {
    g_value_set_string(value, g_strdup(self->lupdate));
  } else {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void jig_data_finalize(GObject* object) {
  JigData* self = JIG_DATA(object);
  if (self->desc) {
    g_free(self->desc);
  }
  if (self->lupdate) {
    g_free(self->lupdate);
  }
  G_OBJECT_CLASS(jig_data_parent_class)->finalize(object);
}

static void jig_data_class_init(JigDataClass* class) {
  GObjectClass* gobject_class = G_OBJECT_CLASS(class);
  gobject_class->finalize = jig_data_finalize;
  gobject_class->set_property = jig_data_set_property;
  gobject_class->get_property = jig_data_get_property;
  jig_data_properties[PROP_DESC] = g_param_spec_string("desc", "string", "string", "", G_PARAM_READWRITE);
  char nm[8];
  for (int i = 0; i < 5; ++i) {
    g_snprintf(nm, 8, "status%d", i);
    jig_data_properties[PROP_STATUS0 + i] = g_param_spec_string(nm, "string", "string", "", G_PARAM_READWRITE);
  }
  jig_data_properties[PROP_LUPDATE] = g_param_spec_string("lupdate", "string", "string", "", G_PARAM_READWRITE);
  g_object_class_install_properties(gobject_class, N_PROPERTIES, jig_data_properties);
}

JigData* jig_data_new_with_data(GtkListItem* item, char* desc,
    char* status0, char* status1, char* status2, char* status3, char* status4, char* lupdate) {
  g_return_val_if_fail(GTK_IS_LIST_ITEM(item) || item == NULL, NULL);
  JigData *data = JIG_DATA(g_object_new(JIG_TYPE_DATA, NULL));
  data->item = item ? g_object_ref(item) : NULL;
  data->desc = g_strdup(desc);
  data->status[0] = g_strdup(status0);
  data->status[1] = g_strdup(status1);
  data->status[2] = g_strdup(status2);
  data->status[3] = g_strdup(status3);
  data->status[4] = g_strdup(status4);
  data->lupdate = g_strdup(lupdate);
  return data;
}

// End of declaration

// Row addition callback
void new_row_cb(GtkButton* btn, TuasWindow* win) {
  JigData* data = jig_data_new_with_data(NULL, "", "—", "—", "—", "—", "—", "");
  g_list_store_append(win->liststore, data);
  g_object_unref(data);
}

// Description callbacks
void setup_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* text = gtk_entry_new();
  gtk_list_item_set_child(item, text);
}

void bind_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkEntry* entry = GTK_ENTRY(gtk_list_item_get_child(item));
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
  // gtk_entry_set_placeholder_text(entry, "Jig description");
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  gtk_editable_set_text(GTK_EDITABLE(entry), data->desc);
  GBinding* bind = g_object_bind_property(buffer, "text", data, "desc", G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

void unbind_desc_cb(GtkListItemFactory* self, GtkListItem* item) {
  GBinding* bind = G_BINDING(g_object_get_data(G_OBJECT(item), "bind"));
  g_binding_unbind(bind);
  g_object_set_data(G_OBJECT(item), "bind", NULL);
}

// Parameter button callbacks
void setup_status_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* btn = gtk_button_new();
  gtk_list_item_set_child(item, btn);
}

void state_change_cb(GtkButton* btn) {
  const char* label = gtk_button_get_label(btn);
  if (g_str_equal(label, "—")) {
    gtk_button_set_label(btn, "Done");
  } else if (g_str_equal(label, "Done")) {
    gtk_button_set_label(btn, "Pending");
  } else if (g_str_equal(label, "Pending")) {
    gtk_button_set_label(btn, "—");
  }
}

void bind_status_cb(GtkListItemFactory* self, GtkListItem* item, gpointer i) {
  GtkButton* btn = GTK_BUTTON(gtk_list_item_get_child(item));
  g_signal_connect(btn, "clicked", G_CALLBACK(state_change_cb), NULL);
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  gtk_button_set_label(btn, data->status[GPOINTER_TO_INT(i)]);
  char nm[8]; g_snprintf(nm, 8, "status%d", GPOINTER_TO_INT(i));
  GBinding* bind = g_object_bind_property(btn, "label", data, g_strdup(nm), G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

void unbind_status_cb(GtkListItemFactory* self, GtkListItem* item) {
  GBinding* bind = G_BINDING(g_object_get_data(G_OBJECT(item), "bind"));
  g_binding_unbind(bind);
  g_object_set_data(G_OBJECT(item), "bind", NULL);
}

// Latest update callbacks (setup and unbind callbacks are shared with desc)
void bind_lupdate_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkEntry* entry = GTK_ENTRY(gtk_list_item_get_child(item));
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
  gtk_entry_set_placeholder_text(entry, "Started");
  JigData* data = JIG_DATA(gtk_list_item_get_item(item));
  gtk_editable_set_text(GTK_EDITABLE(entry), data->lupdate);
  GBinding* bind = g_object_bind_property(buffer, "text", data, "lupdate", G_BINDING_DEFAULT);
  g_object_set_data(G_OBJECT(item), "bind", bind);
}

// Progress label callbacks
void setup_progress_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkWidget* label = gtk_label_new("0 / 5");
  gtk_list_item_set_child(item, label);
}

void progress_label_notify(GObject* item, GParamSpec* pspec, gpointer label) {
  int n_done = 0;
  JigData* data = JIG_DATA(item);
  for (int i = 0; i < 5; ++i) {
    if (g_str_equal(data->status[i], "Done")) {
      ++n_done;
    }
  }
  char nm[6]; g_snprintf(nm, 6, "%d / 5", n_done);
  gtk_label_set_text(GTK_LABEL(label), nm);
}

void bind_progress_cb(GtkListItemFactory* self, GtkListItem* item) {
  GtkLabel* label = GTK_LABEL(gtk_list_item_get_child(item));
  JigData* data = gtk_list_item_get_item(item);
  progress_label_notify(G_OBJECT(data), NULL, label);
  g_signal_connect(data, "notify", G_CALLBACK(progress_label_notify), label);
}

// String processing

char* quote_string(char* in) {
  if (g_strstr_len(in, -1, ",") == NULL && g_strstr_len(in, -1, "\"") == NULL) {
    return in;
  }
  GRegex* quote_re = g_regex_new("\"", 0, 0, NULL);
  return g_strconcat("\"", g_regex_replace(quote_re, in, -1, 0, "\"\"", 0, NULL), "\"", NULL);
}

// Save file callbacks
char label_to_abbrev(char* s) {
  if (g_str_equal(s, "Done")) {
    return 'D';
  } if (g_str_equal(s, "Pending")) {
    return 'P';
  }
  return 'O';
}

void save_cb(GObject* fd, GAsyncResult* res, gpointer listmodel) {
  GFile* file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(fd), res, NULL);
  if (!file) {
    return;
  }
  GListModel* lm = listmodel;
  JigData* item;
  char* text = "";
  for (int n = 0; (item = g_list_model_get_item(lm, n)) != NULL; ++n) {
    char line[6] = {0};
    for (int i = 0; i < 5; ++i) {
      line[i] = label_to_abbrev(item->status[i]);
    }
    text = g_strconcat(text, line, ",", quote_string(item->desc),
                                   ",", quote_string(item->lupdate), "\n", NULL);
  }
  g_file_replace_contents(file, text, strlen(text), NULL, false, G_FILE_CREATE_NONE, NULL, NULL, NULL);
  g_free(text);
}

void save_button_cb(GtkButton* btn, TuasWindow* win) {
  GtkFileDialog* fd = gtk_file_dialog_new();
  gtk_file_dialog_set_initial_folder(fd, g_file_new_for_path(g_get_current_dir()));
  gtk_file_dialog_save(fd, GTK_WINDOW(win), NULL, save_cb, win->liststore);
}

// Open file callbacks
char* abbrev_to_label(char s) {
  switch (s) {
    case 'D': return "Done"; break;
    case 'P': return "Pending"; break;
    case 'O':
     default: return "—"; break;
  }
}

// ! More string processing!

JigData* jig_data_new_from_line(char* ln) {
  char* desc;
  char* comma; // index of the second comma separator
  char* lupdate;
  GRegex* dquote_re = g_regex_new("\"\"", 0, 0, NULL);
  if (ln[6] != '\"') { // description is not quoted
    comma = g_strstr_len(ln + 6, -1, ",");
    desc = g_strndup(ln + 6, comma - (ln + 6));
  } else {
    comma = g_strstr_len(ln + 7, -1, "\",");
    desc = g_regex_replace(dquote_re, g_strndup(ln + 7, comma++ - (ln + 7)), -1, 0, "\"", 0, NULL);
  }
  if (strlen(comma) > 1 && comma[1] == '"') {
    lupdate = g_regex_replace(dquote_re, comma + 2, -1, 0, "\"", 0, NULL);
    lupdate = g_strndup(lupdate, strlen(lupdate) - 1);
  } else {
    lupdate = comma + 1;
  }
  JigData* res = jig_data_new_with_data(NULL, desc,
    abbrev_to_label(ln[0]), abbrev_to_label(ln[1]), abbrev_to_label(ln[2]),
    abbrev_to_label(ln[3]), abbrev_to_label(ln[4]), lupdate);
  return res;
}

void open_cb(GObject* fd, GAsyncResult* res, gpointer liststore) {
  GFile* file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(fd), res, NULL);
  if (!file) {
    return;
  }
  GListStore* lm = liststore;
  g_list_store_remove_all(lm);
  char* raw = NULL;
  g_file_load_contents(file, NULL, &raw, NULL, NULL, NULL);
  char** lines = g_strsplit(raw, "\n", -1);
  g_free(raw);
  for (int i = 0; strlen(lines[i]) > 0; ++i) {
    JigData* data = jig_data_new_from_line(lines[i]);
    g_list_store_append(lm, data);
    g_object_unref(data);
  }
  g_strfreev(lines);
}

void open_button_cb(GtkButton* btn, TuasWindow* win) {
  GtkFileDialog* fd = gtk_file_dialog_new();
  gtk_file_dialog_set_initial_folder(fd, g_file_new_for_path(g_get_current_dir()));
  gtk_file_dialog_open(fd, GTK_WINDOW(win), NULL, open_cb, win->liststore);
}

// Deletion (of completed jigs) callback
void delete_button_cb(GtkButton* btn, TuasWindow* win) {
  GListStore* ls = win->liststore;
  JigData* item;
  for (int n = 0; (item = g_list_model_get_item(G_LIST_MODEL(ls), n)) != NULL;) {
    int has_open = 0;
    for (int i = 0; i < 5; ++i) {
      has_open += g_str_equal(item->status[i], "Open");
    }
    if (has_open > 0) {
      ++n;
    } else {
      g_list_store_remove(ls, n);
    }
  }
}

// Deletion (of everything) callback
void delete_all_button_cb(GtkButton* btn, TuasWindow* win) {
  g_list_store_remove_all(win->liststore);
}

// Begin tedious boilerplate

char* colnames[] = {"FAI", "Check doc", "Report", "Approval", "Print & sign"};

void tuas_window_init(TuasWindow* win) {
  gtk_widget_init_template(GTK_WIDGET(win));
  for (int i = 0; i < 5; ++i) {
    GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_status_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_status_cb), GINT_TO_POINTER(i));
    g_signal_connect(factory, "unbind", G_CALLBACK(unbind_status_cb), NULL);
    GtkColumnViewColumn* column = gtk_column_view_column_new(g_strdup(colnames[i]), factory);
    gtk_column_view_insert_column(win->columnview, i + 1, column);
  }
  GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
  g_signal_connect(factory, "setup", G_CALLBACK(setup_progress_cb), NULL);
  g_signal_connect(factory, "bind", G_CALLBACK(bind_progress_cb), NULL);
  GtkColumnViewColumn* column = gtk_column_view_column_new("Progress", factory);
  gtk_column_view_append_column(win->columnview, column);
}

void tuas_window_class_init(TuasWindowClass* class) {
  GtkWidgetClass* wc = GTK_WIDGET_CLASS(class);
  gtk_widget_class_set_template_from_resource(wc, "/org/smrt/Tuas/window.ui");
  gtk_widget_class_bind_template_child(wc, TuasWindow, columnview);
  gtk_widget_class_bind_template_child(wc, TuasWindow, liststore);
  gtk_widget_class_bind_template_callback(wc, setup_desc_cb);
  gtk_widget_class_bind_template_callback(wc, bind_desc_cb);
  gtk_widget_class_bind_template_callback(wc, unbind_desc_cb);
  gtk_widget_class_bind_template_callback(wc, bind_lupdate_cb);
  gtk_widget_class_bind_template_callback(wc, new_row_cb);
  gtk_widget_class_bind_template_callback(wc, open_button_cb);
  gtk_widget_class_bind_template_callback(wc, save_button_cb);
  gtk_widget_class_bind_template_callback(wc, delete_button_cb);
  gtk_widget_class_bind_template_callback(wc, delete_all_button_cb);
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

int main(int argc, char* argv[]) {
  GtkApplication* app = gtk_application_new("org.smrt.Tuas", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
