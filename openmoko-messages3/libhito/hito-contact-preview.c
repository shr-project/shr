#include <gtk/gtk.h>
#include "hito-contact-preview.h"

G_DEFINE_TYPE (HitoContactPreview, hito_contact_preview, GTK_TYPE_VBOX);

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_CONTACT_PREVIEW, HitoContactPreviewPrivate))

typedef struct _HitoContactPreviewPrivate HitoContactPreviewPrivate;

struct _HitoContactPreviewPrivate
{
  EContact *contact;
};

enum {
  PROP_CONTACT = 1
};

typedef struct {
  char *vcard_field; /* vCard field */
  char *label; /* Display label, NULL for no label */
  char *icon; /* Icon to display next to label */
  char *markup; /* Formatted markup to display content with, or NULL */
} FieldInfo;

static const FieldInfo fields[] = {
  { EVC_FN, NULL, NULL, "<big><b>%s</b></big>" },
  { EVC_EMAIL, "Email:", "stock_mail", NULL },
  { EVC_X_JABBER, "Jabber:", NULL, NULL },
};

static void
hito_contact_preview_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  HitoContactPreviewPrivate *priv = GET_PRIVATE (object);

  switch (property_id) {
  case PROP_CONTACT:
    g_value_set_object (value, priv->contact);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_contact_preview_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
    hito_contact_preview_set_contact (HITO_CONTACT_PREVIEW (object),
                                      g_value_get_object (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_contact_preview_dispose (GObject *object)
{
  if (G_OBJECT_CLASS (hito_contact_preview_parent_class)->dispose)
    G_OBJECT_CLASS (hito_contact_preview_parent_class)->dispose (object);
}

static void
hito_contact_preview_finalize (GObject *object)
{
  G_OBJECT_CLASS (hito_contact_preview_parent_class)->finalize (object);
}

static void
hito_contact_preview_class_init (HitoContactPreviewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoContactPreviewPrivate));

  object_class->get_property = hito_contact_preview_get_property;
  object_class->set_property = hito_contact_preview_set_property;
  object_class->dispose = hito_contact_preview_dispose;
  object_class->finalize = hito_contact_preview_finalize;

  g_object_class_install_property (object_class, PROP_CONTACT,
                                   g_param_spec_object ("contact", "contact", NULL,
                                                        E_TYPE_CONTACT,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));
}

static void
hito_contact_preview_init (HitoContactPreview *self)
{
  gtk_box_set_spacing (GTK_BOX (self), 6);
}

GtkWidget*
hito_contact_preview_new (void)
{
  return g_object_new (HITO_TYPE_CONTACT_PREVIEW, NULL);
}

void
hito_contact_preview_set_contact (HitoContactPreview *preview, EContact *contact)
{
  GtkWidget *box;
  GtkSizeGroup *size;
  GList *attrs, *l;
  EVCardAttribute *attr;
  int i;
  GtkWidget *w;
  char *s, *value;

  gtk_container_foreach (GTK_CONTAINER (preview), (GtkCallback)gtk_widget_destroy, NULL);
  
  if (!contact)
    goto done;

  size = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
  attrs = e_vcard_get_attributes (E_VCARD (contact));

  for (i = 0; i < G_N_ELEMENTS (fields); i++) {
    for (l = attrs; l; l = g_list_next (l)) {
      attr = l->data;

      if (g_ascii_strcasecmp (fields[i].vcard_field, e_vcard_attribute_get_name (attr)) != 0)
        continue;

      box = gtk_hbox_new (FALSE, 4);

      if (fields[i].label) {
        /* TODO: icon */
        /* TODO: make bold */
        w = gtk_label_new (NULL);
        s = g_markup_printf_escaped ("<b>%s</b>",fields[i].label);
        gtk_label_set_markup (GTK_LABEL (w), s);
        g_free (s);
        gtk_size_group_add_widget (size, w);
        gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
        /* TODO: mnenomic */
        gtk_box_pack_start (GTK_BOX (box), w, FALSE, FALSE, 0);
      }
      
      value = e_vcard_attribute_get_value (attr);
      if (fields[i].markup) {
        w = gtk_label_new (NULL);
        s = g_markup_printf_escaped (fields[i].markup, value);
        gtk_label_set_markup (GTK_LABEL (w), s);
        g_free (s);
      } else {
        w = gtk_label_new (value);
      }
      gtk_misc_set_alignment (GTK_MISC (w), 0.0, 0.5);
      gtk_box_pack_start (GTK_BOX (box), w, TRUE, TRUE, 0);

      g_free (value);

      gtk_widget_show_all (box);
      gtk_box_pack_start (GTK_BOX (preview), box, FALSE, FALSE, 0);
    }
  }

  g_object_unref (size);

 done:
  g_object_notify (G_OBJECT (preview), "contact");
}
